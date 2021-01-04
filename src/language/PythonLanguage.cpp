/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PythonLanguage.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 03:38:46 by skybt             #+#    #+#             */
/*   Updated: 2020/05/08 06:08:34 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PythonLanguage.hpp"

PythonLanguage::PythonLanguage() {}
PythonLanguage::~PythonLanguage() {}

static bool PYTHON_HEADERS_SENT = false;
static HTTPHeaders PYTHON_HEADERS = HTTPHeaders();
static bool PYTHON_BODY_EOF = false;

static PyObject* python_header(PyObject* self, PyObject* args) {
  (void)self;

  char const* key;
  char const* value;

  if (!PyArg_ParseTuple(args, "s|s", &key, &value))
    return NULL;

  if (PYTHON_HEADERS_SENT) {
    PyErr_SetString(PyExc_RuntimeError, "headers are already sent!");
    return NULL;
  }

  PYTHON_HEADERS[key] = value;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* python_send_headers(PyObject* self, PyObject* args) {
  (void)self;
  (void)args;

  if (PYTHON_HEADERS_SENT) {
    PyErr_SetString(PyExc_RuntimeError, "headers are already sent!");
    return NULL;
  }

  PyObject* gblStatus = PyDict_GetItemString(PyEval_GetGlobals(), "STATUS");

  Py_ssize_t status = PyNumber_AsSsize_t(gblStatus, NULL);

  if (status != 200)
    std::cout << "Status: " << status << "\r\n";

  std::cout << PYTHON_HEADERS << "\r\n" << std::flush;

  PYTHON_HEADERS_SENT = true;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* python_write(PyObject* self, PyObject* args) {
  (void)self;

  if (!PYTHON_HEADERS_SENT) {
    PyObject* gblSendHeaders = PyDict_GetItemString(PyEval_GetGlobals(), "send_headers");
    PyObject* args = PyTuple_New(0);
    PyObject* ret = PyObject_CallObject(gblSendHeaders, args);
    Py_DECREF(args);
    Py_DECREF(ret);
  }

  char const* str;
  if (!PyArg_ParseTuple(args, "s", &str))
    return NULL;

  std::cout << str;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* python_read(PyObject* self, PyObject* args) {
  (void)self;
  (void)args;

  if (!PYTHON_HEADERS_SENT) {
    PyErr_SetString(PyExc_RuntimeError, "headers are not sent yet!");
    return NULL;
  }

  char buffer[PYTHON_BUFFER_SIZE];

  if (PYTHON_BODY_EOF) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  ssize_t len;
  if ((len = read(0, buffer, PYTHON_BUFFER_SIZE)) < 0)
    return PyErr_Format(PyExc_RuntimeError, "read error (%s)", strerror(errno));

  if (len == 0) {
    PYTHON_BODY_EOF = true;
    Py_INCREF(Py_True);
    PyDict_SetItemString(PyEval_GetGlobals(), "BODY_EOF", Py_True);
    Py_INCREF(Py_None);
    return Py_None;
  }

  std::string str(buffer, buffer + len);
  return PyUnicode_FromString(str.c_str());
}

static PyObject* python_print(PyObject* self, PyObject* args) {
  (void)self;
  PyObject* builtins = PyEval_GetBuiltins();
  PyObject* strFn = PyDict_GetItemString(builtins, "str");

  size_t len = PyTuple_Size(args);
  for (size_t i = 0; i < len; i++) {
    PyObject* obj = PyTuple_GetItem(args, i);

    PyObject* strArgs = PyTuple_New(1);
    PyTuple_SetItem(strArgs, 0, obj);

    PyObject* ret = PyObject_CallObject(strFn, strArgs);
    wchar_t const* str = PyUnicode_AS_UNICODE(ret);

    std::wcerr << str;

    Py_DECREF(strArgs);
    Py_DECREF(ret);

    if (i < len - 1)
      std::wcerr << " ";
  }

  std::wcerr << std::endl;
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef python_webserv_module_methods[] = {
  {"header", &python_header, METH_VARARGS, NULL},
  {"send_headers", &python_send_headers, METH_VARARGS, NULL},
  {"write", &python_write, METH_VARARGS, NULL},
  {"read", &python_read, METH_VARARGS, NULL},
  {"print", &python_print, METH_VARARGS, NULL},
  {NULL, NULL, 0, NULL}
};

static PyModuleDef python_webserv_module = {
  PyModuleDef_HEAD_INIT, "webserv", NULL, -1,
  python_webserv_module_methods,
  NULL, NULL, NULL, NULL
};

static PyObject* python_webserv_module_create() {
  return PyModule_Create(&python_webserv_module);
}

void PythonLanguage::execute(HTTPResponse const& res) {
  wchar_t* name = Py_DecodeLocale(res.getCGIScriptName().c_str(), NULL);
  if (!name) {
    std::cerr << "[Python] Invalid file name!" << std::endl;
    return;
  }

  PyImport_AppendInittab("webserv", &python_webserv_module_create);

  Py_SetProgramName(name);
  Py_Initialize();

  PyObject* mod = PyImport_ImportModule("webserv");
  PyObject* gbl = PyModule_GetDict(mod);
  PyObject* loc = PyDict_New();
  PyDict_SetItemString(gbl, "__builtins__", PyEval_GetBuiltins());

  PyDict_SetItemString(gbl, "PATH_INFO", PyUnicode_FromString(res.getCGIPathInfo().c_str()));
  PyDict_SetItemString(gbl, "PATH_TRANSLATED", PyUnicode_FromString(res.getCGIPathTranslated().c_str()));
  PyDict_SetItemString(gbl, "QUERY_STRING", PyUnicode_FromString(res.getCGIReq().getQueryString().c_str()));
  PyDict_SetItemString(gbl, "REQUEST_METHOD", PyUnicode_FromString(getHTTPMethodName(res.getCGIReq().getMethod())));
  PyDict_SetItemString(gbl, "SCRIPT_NAME", PyUnicode_FromString(res.getCGIScriptName().c_str()));
  PyDict_SetItemString(gbl, "SERVER_NAME", PyUnicode_FromString(res.getCGIServerName().c_str()));
  PyDict_SetItemString(gbl, "SERVER_PORT", PyLong_FromLong(res.getCGIServerPort()));
  PyDict_SetItemString(gbl, "SERVER_PROTOCOL", PyUnicode_FromString("HTTP/1.1"));
  PyDict_SetItemString(gbl, "SERVER_SOFTWARE", PyUnicode_FromString("webserv/1.0"));

  Py_INCREF(Py_None);
  PyDict_SetItemString(gbl, "CONTENT_LENGTH", Py_None);
  Py_INCREF(Py_None);
  PyDict_SetItemString(gbl, "CONTENT_TYPE", Py_None);

  PyObject* pHeaders = PyDict_New();

  for (HTTPHeaders::const_iterator ite = res.getCGIReq().getHeaders().begin();
      ite != res.getCGIReq().getHeaders().end(); ++ite) {

    if (!strcasecmp(ite->first.c_str(), "Content-Length")) {
      PyDict_SetItemString(gbl, "CONTENT_LENGTH", PyUnicode_FromString(ite->second.c_str()));
    } else if (!strcasecmp(ite->first.c_str(), "Content-Type")) {
      PyDict_SetItemString(gbl, "CONTENT_TYPE", PyUnicode_FromString(ite->second.c_str()));
    }

    PyDict_SetItemString(pHeaders, ite->first.c_str(), PyUnicode_FromString(ite->second.c_str()));
  }

  PyDict_SetItemString(gbl, "HEADERS", pHeaders);

  PyDict_SetItemString(gbl, "STATUS", PyLong_FromLong(STATUS_OK));
  Py_INCREF(Py_False);
  PyDict_SetItemString(gbl, "BODY_EOF", Py_False);

  PYTHON_HEADERS["Content-Type"] = "text/html";
  PYTHON_HEADERS["X-Powered-By"] = "Python " PY_VERSION;

  FILE* file = fopen(res.getCGIFilePath().c_str(), "r");
  PyObject* ret = PyRun_File(file, res.getCGIScriptName().c_str(), Py_file_input, gbl, loc);

  if (!ret)
    PyErr_Print();

  if (!PYTHON_HEADERS_SENT) {
    if (!ret)
      PyDict_SetItemString(gbl, "STATUS", PyLong_FromLong(STATUS_INTERNAL_SERVER_ERROR));

    PyObject* gblSendHeaders = PyDict_GetItemString(PyEval_GetGlobals(), "send_headers");
    PyObject* args = PyTuple_New(0);
    PyObject* ret = PyObject_CallObject(gblSendHeaders, args);
    Py_DECREF(args);
    Py_DECREF(ret);
  }

  std::cout << std::flush;

  PYTHON_HEADERS = HTTPHeaders();
  PYTHON_HEADERS_SENT = false;
  PYTHON_BODY_EOF = false;

  fclose(file);
  Py_FinalizeEx();
  PyMem_RawFree(name);
}
