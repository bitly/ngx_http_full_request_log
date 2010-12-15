/*
 * nginx module to log an entire request
 *
 * mattr@bit.ly 2010-12-15
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_flag_t enable;
} ngx_http_full_request_logger_main_conf_t;

typedef struct {
    ngx_flag_t off;
} ngx_http_full_request_logger_loc_conf_t;

static void *ngx_http_full_request_logger_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_full_request_logger_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_full_request_logger_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_full_request_logger_init(ngx_conf_t *cf);

static ngx_command_t ngx_http_full_request_logger_commands[] = {
    { ngx_string("enable"),
      NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      0,
      NULL },
    { ngx_string("off"),
      NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
    ngx_null_command
};

static ngx_http_module_t  ngx_http_full_request_logger_module_ctx = {
    NULL,                                               /* preconfiguration */
    ngx_http_full_request_logger_init,                  /* postconfiguration */
                                                        
    ngx_http_full_request_logger_create_main_conf,      /* create main configuration */
    NULL,                                               /* init main configuration */
                                                        
    NULL,                                               /* create server configuration */
    NULL,                                               /* merge server configuration */
                                                        
    ngx_http_full_request_logger_create_loc_conf,       /* create location configuration */
    ngx_http_full_request_logger_merge_loc_conf         /* merge location configuration */
};

ngx_module_t ngx_http_full_request_logger_module = {
    NGX_MODULE_V1,
    &ngx_http_full_request_logger_module_ctx,   /* module context */
    ngx_http_full_request_logger_commands,      /* module directives */
    NGX_HTTP_MODULE,                            /* module type */
    NULL,                                       /* init master */
    NULL,                                       /* init module */
    NULL,                                       /* init process */
    NULL,                                       /* init thread */
    NULL,                                       /* exit thread */
    NULL,                                       /* exit process */
    NULL,                                       /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_full_request_logger_handler(ngx_http_request_t *r)
{
    ngx_http_full_request_logger_loc_conf_t *lcf;
    
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_full_request_logger_module);
    
    if (lcf->off) {
        return NGX_OK;
    }
    
    return NGX_OK;
}

static ngx_int_t ngx_http_full_request_logger_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt *h;
    ngx_http_full_request_logger_main_conf_t *lmcf;
    ngx_http_core_main_conf_t  *cmcf;
    
    lmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_full_request_logger_module);
    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    
    if (lmcf->enable) {
        h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
        if (h == NULL) {
            return NGX_ERROR;
        }
        
        *h = ngx_http_full_request_logger_handler;
    }
    
    return NGX_OK;
}

static void *ngx_http_full_request_logger_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_full_request_logger_main_conf_t *conf;
    
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_full_request_logger_main_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    
    conf->enable = NGX_CONF_UNSET;
    
    return conf;
}

static void *ngx_http_full_request_logger_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_full_request_logger_loc_conf_t *conf;
    
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_full_request_logger_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    
    conf->off = NGX_CONF_UNSET;
    
    return conf;
}

static char *ngx_http_full_request_logger_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_full_request_logger_loc_conf_t *prev = parent;
    ngx_http_full_request_logger_loc_conf_t *conf = child;
    
    ngx_conf_merge_value(conf->off, prev->off, 0);
    
    return NGX_CONF_OK;
}
