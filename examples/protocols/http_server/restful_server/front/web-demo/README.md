# Dashboard

This is a modernized version of the dashboard web interface.

## Features

- **Home**: Display ESP system information (IDF version, CPU name)
- **Chart**: Real-time temperature data visualization with spark-line chart
- **Light**: RGB light control with color sliders

## Technology Stack

- **Vue 3**: Modern Vue.js framework with Composition API
- **Vuetify 3**: Material Design component library
- **Alova**: Modern HTTP client with better performance and features
- **Pinia**: State management
- **Vite**: Fast build tool and development server

## Performance Optimizations

- **Tree-shaking for icons**: Only imports specific MDI icons used in the app (this is important for deploying web pages to embedded devices like ESP32)
- **Modern bundle size**: Smaller bundle with Vite and modern dependencies
- **Composition API**: Better performance and code organization

## API Endpoints

The application expects the following endpoints from ESP32 device:

- `GET /api/v1/system/info` - System information
- `GET /api/v1/temp/raw` - Temperature sensor data
- `POST /api/v1/light/brightness` - Set RGB LED colors

## Development

```bash
# Install dependencies
pnpm install

# Start development server
pnpm dev

# Build for production
pnpm build
```
