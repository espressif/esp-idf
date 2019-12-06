<template>
  <v-container fluid>
    <v-sparkline
      :value="get_chart_value"
      :gradient="['#f72047', '#ffd200', '#1feaea']"
      :smooth="10"
      :padding="8"
      :line-width="2"
      stroke-linecap="round"
      gradient-direction="top"
      auto-draw
    ></v-sparkline>
  </v-container>
</template>

<script>
export default {
  data() {
    return {
      timer: null
    };
  },
  computed: {
    get_chart_value() {
      return this.$store.state.chart_value;
    }
  },
  methods: {
    updateData: function() {
      this.$store.dispatch("update_chart_value");
    }
  },
  mounted() {
    clearInterval(this.timer);
    this.timer = setInterval(this.updateData, 1000);
  },
  destroyed: function() {
    clearInterval(this.timer);
  }
};
</script>
