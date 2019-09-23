import Vue from 'vue'
import Vuex from 'vuex'
import axios from 'axios'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    chart_value: [8, 2, 5, 9, 5, 11, 3, 5, 10, 0, 1, 8, 2, 9, 0, 13, 10, 7, 16],
  },
  mutations: {
    update_chart_value(state, new_value) {
      state.chart_value.push(new_value);
      state.chart_value.shift();
    }
  },
  actions: {
    update_chart_value({ commit }) {
      axios.get("/api/v1/temp/raw")
        .then(data => {
          commit("update_chart_value", data.data.raw);
        })
        .catch(error => {
          console.log(error);
        });
    }
  }
})
