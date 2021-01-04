
STATUS = 400

header('Content-Type', 'text/plain')

const vars = [
  'PATH_INFO', 'PATH_TRANSLATED',
  'QUERY_STRING', 'REQUEST_METHOD',
  'SCRIPT_NAME', 'SERVER_NAME',
  'SERVER_PORT', 'SERVER_PROTOCOL',
  'SERVER_SOFTWARE', 'CONTENT_LENGTH',
  'CONTENT_TYPE'
]

// other globals: HEADERS, STATUS, BODY_EOF

write('VARIABLES: \n\n')

for (const name of vars) {
    write(`- ${name}: ${globalThis[name]}\n`)
}

write('\n')

// print a debug message to the console
print('Hello from javascript')

write('HEADERS: \n\n')

for (const [k, v] of Object.entries(HEADERS)) {
    write(`- ${k}: ${v}\n`)
}

if (REQUEST_METHOD == 'POST') {
    write('\n')
    write('This is a POST request!\n')
    write('Here are the data sent by the client:\n\n')

    while (true) {
        const s = read()
        if (s == null)
            break
        write(s)
    }
}

