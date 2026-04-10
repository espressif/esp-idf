/**
 * Composables for dashboard application
 */
import { onUnmounted, ref, watch } from 'vue'
import { lightApi, systemApi, tempApi } from '@/services/api'

/**
 * Composable for system information
 */
export function useSystemInfo () {
  const systemInfo = ref(null)
  const loading = ref(false)
  const error = ref(null)

  const fetchSystemInfo = async () => {
    loading.value = true
    error.value = null
    try {
      const response = await systemApi.getInfo().send()
      systemInfo.value = response
    } catch (error_) {
      error.value = error_
      console.error('Failed to fetch system info:', error_)
    } finally {
      loading.value = false
    }
  }

  return {
    systemInfo,
    loading,
    error,
    fetchSystemInfo,
  }
}

/**
 * Composable for temperature data polling
 */
export function useTemperaturePolling (interval = 1000) {
  const isPolling = ref(false)
  const error = ref(null)
  let timer = null

  const startPolling = callback => {
    if (isPolling.value) {
      return
    }

    isPolling.value = true
    timer = setInterval(async () => {
      try {
        // Force fresh request by bypassing cache
        const response = await tempApi.getRaw().send({ force: true })
        callback(response.raw)
        error.value = null
      } catch (error_) {
        error.value = error_
        console.error('Failed to fetch temperature data:', error_)
      }
    }, interval)
  }

  const stopPolling = () => {
    if (timer) {
      clearInterval(timer)
      timer = null
    }
    isPolling.value = false
  }

  onUnmounted(() => {
    stopPolling()
  })

  return {
    isPolling,
    error,
    startPolling,
    stopPolling,
  }
}

/**
 * Composable for light control
 */
export function useLightControl () {
  // Load saved RGB values from localStorage or use defaults
  const savedRed = localStorage.getItem('lightControl.red')
  const savedGreen = localStorage.getItem('lightControl.green')
  const savedBlue = localStorage.getItem('lightControl.blue')

  const red = ref(savedRed ? Number.parseInt(savedRed) : 160)
  const green = ref(savedGreen ? Number.parseInt(savedGreen) : 160)
  const blue = ref(savedBlue ? Number.parseInt(savedBlue) : 160)
  const loading = ref(false)
  const error = ref(null)

  // Watch for changes and save to localStorage
  watch(red, newValue => {
    localStorage.setItem('lightControl.red', newValue.toString())
  })

  watch(green, newValue => {
    localStorage.setItem('lightControl.green', newValue.toString())
  })

  watch(blue, newValue => {
    localStorage.setItem('lightControl.blue', newValue.toString())
  })

  const setColor = async () => {
    loading.value = true
    error.value = null
    try {
      // Ensure RGB values are integers (0-255)
      const colorData = {
        red: Math.round(Math.max(0, Math.min(255, red.value || 0))),
        green: Math.round(Math.max(0, Math.min(255, green.value || 0))),
        blue: Math.round(Math.max(0, Math.min(255, blue.value || 0))),
      }
      console.log('Setting color:', colorData)
      const response = await lightApi.setBrightness(colorData).send()
      console.log('Light control response:', response)
    } catch (error_) {
      error.value = error_
      console.error('Failed to set color:', error_)
    } finally {
      loading.value = false
    }
  }

  return {
    red,
    green,
    blue,
    loading,
    error,
    setColor,
  }
}
