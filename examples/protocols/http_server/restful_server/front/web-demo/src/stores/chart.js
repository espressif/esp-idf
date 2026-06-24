/**
 * Chart data store using Pinia
 */
import { defineStore } from 'pinia'
import { ref } from 'vue'
import { tempApi } from '@/services/api'

export const useChartStore = defineStore('chart', () => {
  const chartValue = ref([8, 2, 5, 9, 5, 11, 3, 5, 10, 0, 1, 8, 2, 9, 0, 13, 10, 7, 16])

  const updateChartValue = async () => {
    try {
      const response = await tempApi.getRaw().send()
      chartValue.value.push(response.raw)
      chartValue.value.shift()
    } catch (error) {
      console.error('Failed to update chart value:', error)
    }
  }

  return {
    chartValue,
    updateChartValue,
  }
})
