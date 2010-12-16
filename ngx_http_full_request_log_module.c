/*
 * nginx module to log the full request contents
 *
 * mattr@bit.ly 2010-12-15
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_open_file_t            *file;
    time_t                      disk_full_time;
    time_t                      error_log_time;
} ngx_http_full_request_log_t;

typedef struct {
    ngx_flag_t enable;
} ngx_http_full_request_log_main_conf_t;

typedef struct {
    ngx_array_t                *logs;
    ngx_open_file_cache_t      *open_file_cache;
    time_t                      open_file_cache_valid;
    ngx_uint_t                  open_file_cache_min_uses;
    ngx_uint_t                  off;
} ngx_http_full_request_log_loc_conf_t;

static void *ngx_http_full_request_log_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_full_request_log_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_full_request_log_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_full_request_log_init(ngx_conf_t *cf);

static ngx_command_t ngx_http_full_request_log_commands[] = {
    { ngx_string("full_request_log_enable"),
      NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      0,
      NULL },
    { ngx_string("full_request_log"),
      NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
    ngx_null_command
};

static ngx_http_module_t  ngx_http_full_request_log_module_ctx = {
    NULL,                                               /* preconfiguration */
    ngx_http_full_request_log_init,                  /* postconfiguration */
                                                        
    ngx_http_full_request_log_create_main_conf,      /* create main configuration */
    NULL,                                               /* init main configuration */
                                                        
    NULL,                                               /* create server configuration */
    NULL,                                               /* merge server configuration */
                                                        
    ngx_http_full_request_log_create_loc_conf,       /* create location configuration */
    ngx_http_full_request_log_merge_loc_conf         /* merge location configuration */
};

ngx_module_t ngx_http_full_request_log_module = {
    NGX_MODULE_V1,
    &ngx_http_full_request_log_module_ctx,   /* module context */
    ngx_http_full_request_log_commands,      /* module directives */
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

static ngx_int_t ngx_http_full_request_log_handler(ngx_http_request_t *r)
{
    ngx_http_full_request_log_loc_conf_t *lcf;
    
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_full_request_log_module);
    
    if (lcf->off) {
        return NGX_OK;
    }
    
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "i could have logged this");
    
    return NGX_OK;
}

static ngx_int_t ngx_http_full_request_log_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt *h;
    ngx_http_full_request_log_main_conf_t *lmcf;
    ngx_http_core_main_conf_t  *cmcf;
    
    lmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_full_request_log_module);
    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    
    if (lmcf->enable) {
        h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
        if (h == NULL) {
            return NGX_ERROR;
        }
        
        *h = ngx_http_full_request_log_handler;
    }
    
    return NGX_OK;
}

static void *ngx_http_full_request_log_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_full_request_log_main_conf_t *conf;
    
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_full_request_log_main_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    
    conf->enable = NGX_CONF_UNSET;
    
    return conf;
}

static void *ngx_http_full_request_log_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_full_request_log_loc_conf_t *conf;
    
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_full_request_log_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    
    conf->off = NGX_CONF_UNSET;
    
    return conf;
}

static char *ngx_http_full_request_log_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_full_request_log_loc_conf_t *prev = parent;
    ngx_http_full_request_log_loc_conf_t *conf = child;
    
    ngx_conf_merge_value(conf->off, prev->off, 0);
    
    return NGX_CONF_OK;
}
