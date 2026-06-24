<template>
  <v-container>
    <v-row justify="center">
      <v-col cols="12" lg="8">
        <v-card class="pa-2">
          <v-card-title class="d-flex align-center py-2">
            <span class="text-h6">Temperature Chart</span>
            <v-spacer />
            <v-chip
              :color="isPolling ? 'success' : 'error'"
              size="small"
              variant="flat"
            >
              {{ isPolling ? 'Live' : 'Offline' }}
            </v-chip>
          </v-card-title>
          <v-card-text class="py-2">
            <v-sparkline
              auto-draw
              :gradient="['#f72047', '#ffd200', '#1feaea']"
              gradient-direction="top"
              height="150"
              :line-width="2"
              :model-value="chartStore.chartValue"
              :padding="4"
              :smooth="10"
              stroke-linecap="round"
            />
            <v-alert
              v-if="error"
              class="mt-2"
              density="compact"
              dismissible
              type="error"
              @click:close="error = null"
            >
              Failed to fetch temperature data
            </v-alert>
          </v-card-text>
          <v-card-actions class="py-2">
            <v-btn
              v-if="!isPolling"
              color="primary"
              size="default"
              @click="startDataPolling"
            >
              <v-icon :icon="mdiPlay" start />
              Start Monitoring
            </v-btn>
            <v-btn
              v-else
              color="error"
              size="default"
              @click="stopPolling"
            >
              <v-icon :icon="mdiStop" start />
              Stop Monitoring
            </v-btn>
          </v-card-actions>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
  import { mdiPlay, mdiStop } from '@mdi/js'
  import { onMounted } from 'vue'
  import { useTemperaturePolling } from '@/composables/useApi'
  import { useChartStore } from '@/stores/chart'

  const chartStore = useChartStore()
  const { isPolling, error, startPolling, stopPolling } = useTemperaturePolling(1000)

  const startDataPolling = () => {
    startPolling(newValue => {
      chartStore.chartValue.push(newValue)
      chartStore.chartValue.shift()
    })
  }

  onMounted(() => {
    startDataPolling()
  })
</script>
