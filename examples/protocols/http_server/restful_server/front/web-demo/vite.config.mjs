import { fileURLToPath, URL } from 'node:url'
import Vue from '@vitejs/plugin-vue'
// Plugins
import AutoImport from 'unplugin-auto-import/vite'
import Fonts from 'unplugin-fonts/vite'
import Components from 'unplugin-vue-components/vite'
import { VueRouterAutoImports } from 'unplugin-vue-router'
import VueRouter from 'unplugin-vue-router/vite'
// Utilities
import { defineConfig } from 'vite'

import Layouts from 'vite-plugin-vue-layouts-next'
import Vuetify, { transformAssetUrls } from 'vite-plugin-vuetify'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    VueRouter(),
    Layouts(),
    Vue({
      template: { transformAssetUrls },
    }),
    // https://github.com/vuetifyjs/vuetify-loader/tree/master/packages/vite-plugin#readme
    Vuetify({
      autoImport: true,
      styles: {
        configFile: 'src/styles/settings.scss',
      },
    }),
    Components(),
    Fonts({
      google: {
        families: [{
          name: 'Roboto',
          styles: 'wght@100;300;400;500;700;900',
        }],
      },
    }),
    AutoImport({
      imports: [
        'vue',
        VueRouterAutoImports,
        {
          pinia: ['defineStore', 'storeToRefs'],
        },
      ],
      eslintrc: {
        enabled: true,
      },
      vueTemplate: true,
    }),
  ],
  optimizeDeps: {
    exclude: [
      'vuetify',
      'vue-router',
      'unplugin-vue-router/runtime',
      'unplugin-vue-router/data-loaders',
      'unplugin-vue-router/data-loaders/basic',
    ],
  },
  define: { 'process.env': {} },
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('src', import.meta.url)),
    },
    extensions: [
      '.js',
      '.json',
      '.jsx',
      '.mjs',
      '.ts',
      '.tsx',
      '.vue',
    ],
  },
  server: {
    port: 3000,
    // Proxy configuration to avoid CORS issues during development
    // All requests to /api/* will be forwarded to the ESP32
    // IMPORTANT: Also strips large headers (especially cookies) to avoid HTTP 431 errors
    proxy: {
      '/api': {
        target: 'http://dashboard.local', // Replace with your ESP32's actual IP
        changeOrigin: true,
        ws: true,
        // Configure proxy to minimize headers for ESP32 compatibility
        configure: (proxy, options) => {
          proxy.on('proxyReq', (proxyReq, req, res) => {
            // CRITICAL: Remove cookies - this is often the largest header!
            proxyReq.removeHeader('cookie')

            // Remove other large/unnecessary headers that might cause 431 error
            proxyReq.removeHeader('user-agent')
            proxyReq.removeHeader('accept-encoding')
            proxyReq.removeHeader('accept-language')
            proxyReq.removeHeader('cache-control')
            proxyReq.removeHeader('pragma')
            proxyReq.removeHeader('referer')
            proxyReq.removeHeader('origin')
            proxyReq.removeHeader('sec-fetch-dest')
            proxyReq.removeHeader('sec-fetch-mode')
            proxyReq.removeHeader('sec-fetch-site')
            proxyReq.removeHeader('sec-fetch-user')
            proxyReq.removeHeader('sec-ch-ua')
            proxyReq.removeHeader('sec-ch-ua-mobile')
            proxyReq.removeHeader('sec-ch-ua-platform')
            proxyReq.removeHeader('sec-ch-ua-full-version')
            proxyReq.removeHeader('sec-ch-ua-full-version-list')
            proxyReq.removeHeader('upgrade-insecure-requests')
            proxyReq.removeHeader('x-requested-with')
            proxyReq.removeHeader('dnt')
            proxyReq.removeHeader('te')

            // Keep only absolutely essential headers
            proxyReq.setHeader('host', 'dashboard.local')
            proxyReq.setHeader('accept', 'application/json')

            // For POST requests, keep content-type if it exists
            if (req.method === 'POST' && req.headers['content-type']) {
              proxyReq.setHeader('content-type', req.headers['content-type'])
            }

            const finalHeaders = proxyReq.getHeaders()

            // Log header info for monitoring ESP32 compatibility
            const headerSize = Object.entries(finalHeaders)
              .reduce((size, [key, value]) => size + key.length + value.length + 4, 0)
            console.log(`ESP32 Request: ${req.method} ${req.url} (${Object.keys(finalHeaders).length} headers, ~${headerSize} bytes)`)
          })
        },
      },
    },
  },
  css: {
    preprocessorOptions: {
      sass: {
        api: 'modern-compiler',
      },
      scss: {
        api: 'modern-compiler',
      },
    },
  },
})
