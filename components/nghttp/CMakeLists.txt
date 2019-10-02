set(srcs 
    "nghttp2/lib/nghttp2_buf.c"
    "nghttp2/lib/nghttp2_callbacks.c"
    "nghttp2/lib/nghttp2_debug.c"
    "nghttp2/lib/nghttp2_frame.c"
    "nghttp2/lib/nghttp2_hd.c"
    "nghttp2/lib/nghttp2_hd_huffman.c"
    "nghttp2/lib/nghttp2_hd_huffman_data.c"
    "nghttp2/lib/nghttp2_helper.c"
    "nghttp2/lib/nghttp2_http.c"
    "nghttp2/lib/nghttp2_map.c"
    "nghttp2/lib/nghttp2_mem.c"
    "nghttp2/lib/nghttp2_npn.c"
    "nghttp2/lib/nghttp2_option.c"
    "nghttp2/lib/nghttp2_outbound_item.c"
    "nghttp2/lib/nghttp2_pq.c"
    "nghttp2/lib/nghttp2_priority_spec.c"
    "nghttp2/lib/nghttp2_queue.c"
    "nghttp2/lib/nghttp2_rcbuf.c"
    "nghttp2/lib/nghttp2_session.c"
    "nghttp2/lib/nghttp2_stream.c"
    "nghttp2/lib/nghttp2_submit.c"
    "nghttp2/lib/nghttp2_version.c"
    "port/http_parser.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS port/include nghttp2/lib/includes
                    PRIV_INCLUDE_DIRS private_include)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DHAVE_CONFIG_H")
