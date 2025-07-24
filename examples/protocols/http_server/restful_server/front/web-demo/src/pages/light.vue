<template>
  <v-container>
    <v-row justify="center">
      <v-col cols="12" sm="6">
        <v-card>
          <v-responsive
            class="d-flex align-center justify-center"
            height="300"
            :style="{ background: `rgb(${red}, ${green}, ${blue})` }"
          >
            <div class="text-center">
              <h3 class="text-white text-shadow">
                RGB({{ red }}, {{ green }}, {{ blue }})
              </h3>
            </div>
          </v-responsive>
          <v-card-text>
            <v-container fluid>
              <div class="mb-4">
                <v-row align="center" no-gutters>
                  <v-col class="text-body-1 font-weight-medium" cols="2">
                    Red
                  </v-col>
                  <v-col class="px-3" cols="7">
                    <v-slider
                      v-model="red"
                      color="red"
                      hide-details
                      :max="255"
                      :min="0"
                      :step="1"
                      thumb-label
                    />
                  </v-col>
                  <v-col cols="3">
                    <v-text-field
                      v-model.number="red"
                      density="compact"
                      hide-details
                      :max="255"
                      :min="0"
                      :step="1"
                      type="number"
                    />
                  </v-col>
                </v-row>
              </div>
              <div class="mb-4">
                <v-row align="center" no-gutters>
                  <v-col class="text-body-1 font-weight-medium" cols="2">
                    Green
                  </v-col>
                  <v-col class="px-3" cols="7">
                    <v-slider
                      v-model="green"
                      color="green"
                      hide-details
                      :max="255"
                      :min="0"
                      :step="1"
                      thumb-label
                    />
                  </v-col>
                  <v-col cols="3">
                    <v-text-field
                      v-model.number="green"
                      density="compact"
                      hide-details
                      :max="255"
                      :min="0"
                      :step="1"
                      type="number"
                    />
                  </v-col>
                </v-row>
              </div>
              <div class="mb-4">
                <v-row align="center" no-gutters>
                  <v-col class="text-body-1 font-weight-medium" cols="2">
                    Blue
                  </v-col>
                  <v-col class="px-3" cols="7">
                    <v-slider
                      v-model="blue"
                      color="blue"
                      hide-details
                      :max="255"
                      :min="0"
                      :step="1"
                      thumb-label
                    />
                  </v-col>
                  <v-col cols="3">
                    <v-text-field
                      v-model.number="blue"
                      density="compact"
                      hide-details
                      :max="255"
                      :min="0"
                      :step="1"
                      type="number"
                    />
                  </v-col>
                </v-row>
              </div>
            </v-container>
          </v-card-text>
          <v-card-actions class="justify-center">
            <v-btn
              color="red-accent-4"
              :loading="loading"
              prepend-icon="$check"
              size="large"
              @click="setColor"
            >
              Apply Color
            </v-btn>
          </v-card-actions>
          <v-alert
            v-if="error"
            class="ma-4"
            dismissible
            type="error"
            @click:close="error = null"
          >
            Failed to set light color
          </v-alert>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
  import { useLightControl } from '@/composables/useApi'

  const { red, green, blue, loading, error, setColor } = useLightControl()
</script>

<style scoped>
.text-shadow {
  text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.7);
}
</style>
