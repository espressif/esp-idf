/**
 * plugins/alova.js
 *
 * Alova HTTP client configuration
 */
import { alova } from '@/services/api'

export default function alovaPlugin (app) {
  app.config.globalProperties.$alova = alova
}
