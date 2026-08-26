<template>
  <v-container>
    <v-row justify="center">
      <v-col cols="12" sm="6">
        <v-card :loading="loading">
          <v-img
            contain
            height="200"
            :src="logoSrc"
          />
          <v-card-title class="justify-center">
            <div class="text-center">
              <div class="text-grey">
                Chip: {{ systemInfo?.chip || 'Loading...' }}
              </div>
              <div class="text-grey">
                CPU cores: {{ systemInfo?.cores || 'Loading...' }}
              </div>
              <div class="text-grey">
                IDF version: {{ systemInfo?.idf_version || 'Loading...' }}
              </div>
              <v-alert
                v-if="error"
                class="mt-4"
                dismissible
                type="error"
                @click:close="error = null"
              >
                Failed to load system information
              </v-alert>
            </div>
          </v-card-title>
          <v-card-actions class="justify-center">
            <v-btn
              color="primary"
              :loading="loading"
              @click="fetchSystemInfo"
            >
              Refresh
            </v-btn>
          </v-card-actions>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
  import { onMounted, ref } from 'vue'
  import logoImage from '@/assets/logo.png'
  import { useSystemInfo } from '@/composables/useApi'

  const { systemInfo, loading, error, fetchSystemInfo } = useSystemInfo()
  const logoSrc = logoImage

  onMounted(() => {
    fetchSystemInfo()
  })
</script>
