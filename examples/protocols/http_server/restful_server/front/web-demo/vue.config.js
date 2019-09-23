module.exports = {
  devServer: {
    proxy: {
      '/api': {
        target: 'http://esp-home.local:80',
        changeOrigin: true,
        ws: true
      }
    }
  }
}
