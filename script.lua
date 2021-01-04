
STATUS = 400

header('Content-Type', 'text/plain')

vars = {
  'PATH_INFO', 'PATH_TRANSLATED',
  'QUERY_STRING', 'REQUEST_METHOD',
  'SCRIPT_NAME', 'SERVER_NAME',
  'SERVER_PORT', 'SERVER_PROTOCOL',
  'SERVER_SOFTWARE', 'CONTENT_LENGTH',
  'CONTENT_TYPE'
}

-- other globals: HEADERS, STATUS, BODY_EOF

write('VARIABLES: \n\n')

for _, val in ipairs(vars) do
  write(string.format('- %s: %s\n', val, _G[val]))
end

write('\n')

-- print a debug message to the console
-- print('debug message')

write('HEADERS: \n\n')

for key, value in pairs(HEADERS) do
  write(string.format('- %s: %s\n', key, value))
end
