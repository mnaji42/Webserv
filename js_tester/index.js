const fetch = require('node-fetch')
const fs = require('fs/promises')
const childProcess = require('child_process')

let children = []

async function waitWebservReady(child) {
  return new Promise(res => {
    let msg = ''
    child.stdout.on('data', data => {
      msg += data
      if (msg.includes('Listening'))
        res()

      let index
      while ((index = msg.indexOf('\n')) >= 0) {
        console.log('>== ' + msg.slice(0, index))
        msg = msg.slice(index + 1)
      }
    })
    child.stdout.on('close', data => {
      if (msg.length > 0)
        console.log('>== ' + msg)
    })
  })
}

async function runTest(name, config, fn) {
  console.log(`=====>>> Test: ${name} <<<=====`)

  await Promise.all((await fs.readdir('.'))
    .map(async f => {
      if (f === 'webserv')
        return

      const stat = await fs.stat(f)

      if (stat.isDirectory())
        await fs.rmdir(f, { recursive: true })
      else
        await fs.unlink(f)
    }))

  await fs.writeFile('webserv.conf', config)
  
  const child = childProcess.exec('./webserv webserv.conf')
  try {

    children.push(child)
    await waitWebservReady(child)

    await fn()

    console.log('Everything is OK!')

  } finally {
    child.kill()
  }
}

async function testFetch(name, url, opt, { expectStatus, expectText, showText = false }) {
  let res

  console.log(name)

  try {
    res = await fetch(url, opt)
  } catch (e) {
    throw new Error(`fetch '${name}' error: ${e.message}`)
  }

  if (expectStatus && res.status != expectStatus)
    throw new Error(`fetch '${name}': Response code was not ${expectStatus} (got ${res.status})!`)

  const text = await res.text()
  if (expectText && text !== expectText)
    throw new Error(`fetch '${name}': Response body did not match!`)

  if (showText)
    console.log(`Result ${name}: ${text}`)
}

async function main() {
  process.chdir('./workdir')
  const cwd = process.cwd()

  // ===

  await runTest('getting files', `
    workers 1;

    http {
      server {
        listen 8080;
        root ${cwd};
        auto_index on;
      }
    }
  `, async () => {
    const expected = 'Hello world! I\'m a simple file! What is up?'

    await testFetch('GET /', 'http://localhost:8080/', {}, {
      expectStatus: 200, showText: true
    })

    await fs.writeFile('simple', expected)
    await testFetch('GET /simple', 'http://localhost:8080/simple', {}, {
      expectStatus: 200, expectText: expected
    })

    await testFetch('GET /does_not_exist', 'http://localhost:8080/does_not_exist', {}, {
      expectStatus: 404, showText: true
    })

    await fs.writeFile('bad_perm', expected, { mode: 0o000 })
    await testFetch('GET /bad_perm', 'http://localhost:8080/bad_perm', {}, {
      expectStatus: 500
    })

    await fs.writeFile('with spaces and stuff! ?==_)(\\\'', expected)
    await testFetch('GET /with%20spaces%20and%20stuff%21%20%3F%3D%3D_%29%28%5C%27', 'http://localhost:8080/with%20spaces%20and%20stuff%21%20%3F%3D%3D_%29%28%5C%27', {}, {
      expectStatus: 200, expectText: expected
    })
  })

  // ===

  await runTest('put test', `
    workers 1;

    http {
      server {
        listen 8080;
        root ${cwd};
        auto_index on;
        methods PUT;
      }
    }
  `, async () => {
    const expected = 'Hello, this is some text!'

    await testFetch('PUT /', 'http://localhost:8080/', {
      method: 'PUT', body: expected
    }, {
      expectStatus: 500
    })

    await testFetch('PUT /unicorn', 'http://localhost:8080/unicorn', {
      method: 'PUT', body: expected
    }, {
      expectStatus: 204
    })
    if ((await fs.readFile('unicorn', 'utf-8')) !== expected)
      throw new Error(`File content did not match!`)

    await testFetch('GET /unicorn', 'http://localhost:8080/unicorn', {
      method: 'GET'
    }, {
      expectStatus: 405
    })

    let big = 'unicorn'
    for (let i = 0; i < 20; i++)
      big = big + big

    await testFetch('PUT /big', 'http://localhost:8080/big', {
      method: 'PUT', body: big
    }, {
      expectStatus: 204
    })
    if ((await fs.readFile('big', 'utf-8')) !== big)
      throw new Error(`File content did not match!`)

    const expected2 = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhh'

    await testFetch('PUT /unicorn', 'http://localhost:8080/unicorn', {
      method: 'PUT', body: expected2
    }, {
      expectStatus: 204
    })
    if ((await fs.readFile('unicorn', 'utf-8')) !== expected2)
      throw new Error(`File content did not match!`)

    await testFetch('PUT /unicorn', 'http://localhost:8080/unicorn', {
      method: 'PUT', body: expected
    }, {
      expectStatus: 204
    })
    if ((await fs.readFile('unicorn', 'utf-8')) !== expected)
      throw new Error(`File content did not match!`)

    await testFetch('PUT /deeply/nested/path', 'http://localhost:8080/deeply/nested/path', {
      method: 'PUT', body: expected
    }, {
      expectStatus: 204
    })
    if ((await fs.readFile('deeply/nested/path', 'utf-8')) !== expected)
      throw new Error(`File content did not match!`)
  })

}

main()
  .catch(e => {
    console.error(e)
    children.forEach(child => child.kill())
    process.exit(1)
  })
