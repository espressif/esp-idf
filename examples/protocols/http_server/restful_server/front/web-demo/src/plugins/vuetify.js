/**
 * plugins/vuetify.js
 *
 * Framework documentation: https://vuetifyjs.com`
 */

import {
  mdiChartLine,
  mdiCheck,
  mdiHome,
  mdiLightbulb,
  mdiMenu,
} from '@mdi/js'

// Composables
import { createVuetify } from 'vuetify'

// Icons
import { aliases, mdi } from 'vuetify/iconsets/mdi-svg'
// Styles
import 'vuetify/styles'

// Custom icon aliases for on-demand loading
const customAliases = {
  ...aliases,
  'home': mdiHome,
  'chart-line': mdiChartLine,
  'lightbulb': mdiLightbulb,
  'check': mdiCheck,
  'menu': mdiMenu,
}

// https://vuetifyjs.com/en/introduction/why-vuetify/#feature-guides
export default createVuetify({
  icons: {
    defaultSet: 'mdi',
    aliases: customAliases,
    sets: {
      mdi,
    },
  },
  theme: {
    defaultTheme: 'light',
  },
})
