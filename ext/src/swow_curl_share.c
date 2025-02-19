/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Pierrick Charron <pierrick@php.net>                          |
   +----------------------------------------------------------------------+
*/

/* php-src: f4dbe2390db6116d4867456396e3c97ceb15ff71 */

// @see: https://github.com/php/php-src/pull/13347
#if !defined(__cplusplus) && !defined(_MSC_VER) && defined(HAVE_WTYPEDEF_REDEFINITION)
#pragma GCC diagnostic ignored "-Wtypedef-redefinition"
#endif

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "swow.h"

#include "swow_curl_private.h"

#include <curl/curl.h>

#define SAVE_CURLSH_ERROR(__handle, __err) (__handle)->err.no = (int) __err;

/* {{{ Initialize a share curl handle */
PHP_FUNCTION(swow_curl_share_init)
{
    php_curlsh *sh;

    ZEND_PARSE_PARAMETERS_NONE();

    object_init_ex(return_value, swow_curl_share_ce);
    sh = Z_CURL_SHARE_P(return_value);

    sh->share = curl_share_init();
}
/* }}} */

/* {{{ Close a set of cURL handles */
PHP_FUNCTION(swow_curl_share_close)
{
    zval *z_sh;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_OBJECT_OF_CLASS(z_sh, swow_curl_share_ce)
    ZEND_PARSE_PARAMETERS_END();
}
/* }}} */

static bool _php_curl_share_setopt(php_curlsh *sh, zend_long option, zval *zvalue, zval *return_value) /* {{{ */
{
    CURLSHcode error = CURLSHE_OK;

    switch (option) {
        case CURLSHOPT_SHARE:
        case CURLSHOPT_UNSHARE:
            error = curl_share_setopt(sh->share, option, zval_get_long(zvalue));
            break;

        default:
            zend_argument_value_error(2, "is not a valid cURL share option");
            error = CURLSHE_BAD_OPTION;
            break;
    }

    SAVE_CURLSH_ERROR(sh, error);

    return error == CURLSHE_OK;
}
/* }}} */

/* {{{ Set an option for a cURL transfer */
PHP_FUNCTION(swow_curl_share_setopt)
{
    zval       *z_sh, *zvalue;
    zend_long        options;
    php_curlsh *sh;

    ZEND_PARSE_PARAMETERS_START(3,3)
        Z_PARAM_OBJECT_OF_CLASS(z_sh, swow_curl_share_ce)
        Z_PARAM_LONG(options)
        Z_PARAM_ZVAL(zvalue)
    ZEND_PARSE_PARAMETERS_END();

    sh = Z_CURL_SHARE_P(z_sh);

    if (_php_curl_share_setopt(sh, options, zvalue, return_value)) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ Return an integer containing the last share curl error number */
PHP_FUNCTION(swow_curl_share_errno)
{
    zval        *z_sh;
    php_curlsh  *sh;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_OBJECT_OF_CLASS(z_sh, swow_curl_share_ce)
    ZEND_PARSE_PARAMETERS_END();

    sh = Z_CURL_SHARE_P(z_sh);

    RETURN_LONG(sh->err.no);
}
/* }}} */


/* {{{ return string describing error code */
PHP_FUNCTION(swow_curl_share_strerror)
{
    zend_long code;
    const char *str;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_LONG(code)
    ZEND_PARSE_PARAMETERS_END();

    str = curl_share_strerror(code);
    if (str) {
        RETURN_STRING(str);
    } else {
        RETURN_NULL();
    }
}
/* }}} */

/* CurlShareHandle class */

static zend_object_handlers swow_curl_share_handlers;

static zend_object *swow_curl_share_create_object(zend_class_entry *class_type) {
    php_curlsh *intern = zend_object_alloc(sizeof(php_curlsh), class_type);

    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
#if PHP_VERSION_ID < 80300
    intern->std.handlers = &swow_curl_share_handlers;
#endif

    return &intern->std;
}

static zend_function *swow_curl_share_get_constructor(zend_object *object) {
    zend_throw_error(NULL, "Cannot directly construct CurlShareHandle, use curl_share_init() instead");
    return NULL;
}

static void swow_curl_share_free_obj(zend_object *object)
{
    php_curlsh *sh = swow_curl_share_from_obj(object);

    curl_share_cleanup(sh->share);
    zend_object_std_dtor(&sh->std);
}

void swow_curl_share_register_handlers(void) {
    swow_curl_share_ce->create_object = swow_curl_share_create_object;
#if PHP_VERSION_ID >= 80300
    swow_curl_share_ce->default_object_handlers = &swow_curl_share_handlers;
#endif

    memcpy(&swow_curl_share_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    swow_curl_share_handlers.offset = XtOffsetOf(php_curlsh, std);
    swow_curl_share_handlers.free_obj = swow_curl_share_free_obj;
    swow_curl_share_handlers.get_constructor = swow_curl_share_get_constructor;
    swow_curl_share_handlers.clone_obj = NULL;
    swow_curl_share_handlers.compare = zend_objects_not_comparable;
}
