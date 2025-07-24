<template>
  <v-app>
    <v-navigation-drawer v-model="drawer" app clipped>
      <v-list density="compact">
        <v-list-item
          prepend-icon="$home"
          title="Home"
          to="/"
        />
        <v-list-item
          prepend-icon="$chart-line"
          title="Chart"
          to="/chart"
        />
        <v-list-item
          prepend-icon="$lightbulb"
          title="Light"
          to="/light"
        />
      </v-list>
    </v-navigation-drawer>

    <v-app-bar app clipped-left color="blue-grey-darken-3">
      <v-app-bar-nav-icon @click="drawer = !drawer" />
      <v-app-bar-title>Dashboard</v-app-bar-title>
    </v-app-bar>

    <v-main>
      <v-container fill-height fluid>
        <router-view />
      </v-container>
    </v-main>

    <v-footer app class="px-4 py-3" color="blue-grey-darken-4">
      <v-row align="center" justify="space-between" no-gutters>
        <v-col cols="auto">
          <span class="text-grey-lighten-3 text-body-2">
            Copyright &copy; {{ new Date().getFullYear() }} Espressif Systems. All rights reserved.
          </span>
        </v-col>
        <v-col cols="auto">
          <v-btn
            class="mr-2"
            color="grey-lighten-3"
            href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html"
            :icon="mdiBookOpenPageVariant"
            rel="noopener noreferrer"
            size="small"
            target="_blank"
            variant="text"
          />
          <v-btn
            color="grey-lighten-3"
            href="https://github.com/espressif/esp-idf"
            :icon="mdiGithub"
            rel="noopener noreferrer"
            size="small"
            target="_blank"
            variant="text"
          />
        </v-col>
      </v-row>
    </v-footer>
  </v-app>
</template>

<script setup>
  import { mdiBookOpenPageVariant, mdiGithub } from '@mdi/js'
  import { ref } from 'vue'

  const drawer = ref(null)
</script>
