/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaLanguage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 16:57:48 by skybt             #+#    #+#             */
/*   Updated: 2020/06/07 09:31:12 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LuaLanguage.hpp"

// see https://www.lua.org/pil/24.1.html

static bool LUA_HEADERS_SENT = false;
static HTTPHeaders LUA_HEADERS = HTTPHeaders();
static bool LUA_BODY_EOF = false;

LuaLanguage::LuaLanguage() {}
LuaLanguage::~LuaLanguage() {}

static void lua_set_global_str(lua_State* lua, char const* k, char const* v) {
  lua_pushstring(lua, v);
  lua_setglobal(lua, k);
}

static void lua_set_global_num(lua_State* lua, char const* k, double v) {
  lua_pushnumber(lua, v);
  lua_setglobal(lua, k);
}

static void lua_set_global_cfn(lua_State* lua, char const* k, lua_CFunction v) {
  lua_pushcfunction(lua, v);
  lua_setglobal(lua, k);
}

static void lua_set_global_nil(lua_State* lua, char const* k) {
  lua_pushnil(lua);
  lua_setglobal(lua, k);
}

static void lua_set_global_bool(lua_State* lua, char const* k, bool v) {
  lua_pushboolean(lua, v);
  lua_setglobal(lua, k);
}

static int lua_header(lua_State* lua) {
  char const* key = lua_tostring(lua, 1);
  char const* value = lua_tostring(lua, 2);

  if (LUA_HEADERS_SENT)
    return luaL_error(lua, "headers are already sent!\n");

  LUA_HEADERS[key] = value;
  return 0;
}

static int lua_send_headers(lua_State* lua) {
  if (LUA_HEADERS_SENT)
    return luaL_error(lua, "headers are already sent!\n");

  lua_getglobal(lua, "STATUS");
  if (!lua_isnumber(lua, -1))
    return luaL_error(lua, "STATUS must be an integer!\n");
  int status = (int)lua_tonumber(lua, -1);

  if (status != STATUS_OK)
    std::cout << "Status: " << status << "\r\n";

  std::cout << LUA_HEADERS << "\r\n" << std::flush;

  LUA_HEADERS_SENT = true;
  return 0;
}

static int lua_write(lua_State* lua) {
  if (!LUA_HEADERS_SENT) {
    lua_getglobal(lua, "send_headers");
    lua_call(lua, 0, 0);
  }

  char const* str = lua_tostring(lua, 1);

  std::cout << str << std::flush;
  return 0;
}

static int lua_read(lua_State* lua) {
  char buffer[LUA_BUFFER_SIZE];

  if (LUA_BODY_EOF) {
    lua_pushnil(lua);
    return 1;
  }

  ssize_t len;
  if ((len = read(0, buffer, LUA_BUFFER_SIZE)) < 0)
    return luaL_error(lua, "read error (%s)\n", strerror(errno));

  if (len == 0) {
    LUA_BODY_EOF = true;
    lua_set_global_bool(lua, "BODY_EOF", true);
    lua_pushnil(lua);
    return 1;
  }

  lua_pushlstring(lua, buffer, len);
  return 1;
}

static int lua_print(lua_State* lua) {
  int top = lua_gettop(lua);
  for (int idx = 1; idx <= top; idx++) {
    lua_getglobal(lua, "tostring");
    lua_tocfunction(lua, 1);
    lua_pushvalue(lua, idx);
    lua_call(lua, 1, 1);
    char const* str = lua_tostring(lua, -1);
    std::cerr << str;

    if (idx < top)
      std::cerr << " ";
  }

  std::cerr << std::endl;
  return 0;
}

void LuaLanguage::execute(HTTPResponse const& res) {

  lua_State* lua = luaL_newstate();
  luaopen_base(lua);
  luaopen_table(lua);
  luaopen_io(lua);
  luaopen_string(lua);
  luaopen_math(lua);
  luaL_openlibs(lua);

  lua_set_global_str(lua, "PATH_INFO", res.getCGIPathInfo().c_str());
  lua_set_global_str(lua, "PATH_TRANSLATED", res.getCGIPathTranslated().c_str());
  lua_set_global_str(lua, "QUERY_STRING",
    res.getCGIReq().getQueryString().c_str());
  lua_set_global_str(lua, "REQUEST_METHOD",
    getHTTPMethodName(res.getCGIReq().getMethod()));
  lua_set_global_str(lua, "SCRIPT_NAME", res.getCGIScriptName().c_str());
  lua_set_global_str(lua, "SERVER_NAME", res.getCGIServerName().c_str());
  lua_set_global_num(lua, "SERVER_PORT", res.getCGIServerPort());
  lua_set_global_str(lua, "SERVER_PROTOCOL", "HTTP/1.1");
  lua_set_global_str(lua, "SERVER_SOFTWARE", "webserv/1.0");

  lua_set_global_nil(lua, "CONTENT_LENGTH");
  lua_set_global_nil(lua, "CONTENT_TYPE");

  lua_newtable(lua);

  for (HTTPHeaders::const_iterator ite = res.getCGIReq().getHeaders().begin();
      ite != res.getCGIReq().getHeaders().end(); ++ite) {

    if (!strcasecmp(ite->first.c_str(), "Content-Length")) {
      lua_set_global_str(lua, "CONTENT_LENGTH", ite->second.c_str());
    } else if (!strcasecmp(ite->first.c_str(), "Content-Type")) {
      lua_set_global_str(lua, "CONTENT_TYPE", ite->second.c_str());
    }

    lua_pushstring(lua, ite->first.c_str());
    lua_pushstring(lua, ite->second.c_str());
    lua_settable(lua, -3);
  }

  lua_setglobal(lua, "HEADERS");

  lua_set_global_num(lua, "STATUS", STATUS_OK);
  lua_set_global_bool(lua, "BODY_EOF", false);

  lua_set_global_cfn(lua, "header", &lua_header);
  lua_set_global_cfn(lua, "send_headers", &lua_send_headers);
  lua_set_global_cfn(lua, "write", &lua_write);
  lua_set_global_cfn(lua, "read", &lua_read);
  lua_set_global_cfn(lua, "print", &lua_print);

  LUA_HEADERS["Content-Type"] = "text/html";
  LUA_HEADERS["X-Powered-By"] = LUA_VERSION;

  int ret = luaL_dofile(lua, res.getCGIFilePath().c_str());
  switch (ret) {
    case LUA_ERRFILE:
      std::cerr << "Lua loading error: " << lua_tostring(lua, -1) << std::endl;
      break;
    case LUA_YIELD:
      std::cerr << "Lua exec error: " << lua_tostring(lua, -1) << std::endl;
      break;
    case LUA_ERRRUN:
      std::cerr << "Lua runtime error: " << lua_tostring(lua, -1) << std::endl;
      break;
    case LUA_ERRMEM:
      std::cerr << "Lua allocation error: " << lua_tostring(lua, -1) << std::endl;
      break;
    case LUA_ERRERR:
      std::cerr << "Lua error while running the message handler: " << lua_tostring(lua, -1) << std::endl;
      break;
    case LUA_ERRGCMM:
      std::cerr << "Lua error while running a __gc metamethod: " << lua_tostring(lua, -1) << std::endl;
      break;
  }

  if (!LUA_HEADERS_SENT) {
    if (ret != LUA_OK)
      lua_set_global_num(lua, "STATUS", STATUS_INTERNAL_SERVER_ERROR);
    lua_send_headers(lua);
  }
  
  std::cout << std::flush;

  LUA_HEADERS = HTTPHeaders();
  LUA_HEADERS_SENT = false;
  LUA_BODY_EOF = false;
  lua_close(lua);
}
