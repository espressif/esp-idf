/**
 * API service using Alova
 *
 * CORS Solution: Use empty baseURL to leverage Vite proxy (no CORS issues)
 *
 * The Vite proxy in vite.config.mjs handles '/api' routes by forwarding them
 * to the ESP32, avoiding CORS issues during development.
 */
import { createAlova } from 'alova'
import fetch from 'alova/fetch'
import VueHook from 'alova/vue'

export const alova = createAlova({
  statesHook: VueHook,
  requestAdapter: fetch(),
  baseURL: '',
  beforeRequest (method) {
    // Minimize headers to avoid ESP32 431 error
    const essentialHeaders = {}

    // Only add Content-Type for POST requests
    if (method.type === 'POST') {
      essentialHeaders['Content-Type'] = 'application/json'
    }

    // Clear any existing headers and set only essential ones
    method.config.headers = essentialHeaders
  },
  responded: {
    onSuccess: async response => {
      if (response.status >= 400) {
        throw new Error(`HTTP Error: ${response.status}`)
      }

      // Check if response has content and is JSON
      const contentType = response.headers.get('content-type')
      if (contentType && contentType.includes('application/json')) {
        return response.json()
      } else {
        // For non-JSON responses (like light control), return text or empty object
        const text = await response.text()
        console.log('Non-JSON response:', text)
        return text || { success: true }
      }
    },
    onError: error => {
      console.error('API Error:', error)
      throw error
    },
  },
})

// API endpoints
export const systemApi = {
  getInfo: () => alova.Get('/api/v1/system/info'),
}

export const tempApi = {
  getRaw: () => {
    // Create a fresh request each time with timestamp to prevent caching
    return alova.Get(`/api/v1/temp/raw?_t=${Date.now()}`, {
      localCache: 0, // Disable local cache
      hitSource: 'network', // Always fetch from network
    })
  },
}

export const lightApi = {
  setBrightness: data => alova.Post('/api/v1/light/brightness', data),
}
