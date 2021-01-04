#!/usr/bin/env node

console.log('Content-Type: text/plain')
console.log()

Object.entries(process.env)
  .map(([key, value]) => {
    console.log(`${key}: ${value}`)
  })
