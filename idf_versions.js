var DOCUMENTATION_VERSIONS = {
  DEFAULTS: {
    has_targets: false,
    supported_targets: ["esp32"]
  },
  VERSIONS: [{
      name: "latest",
      has_targets: true,
      supported_targets: ["esp32", "esp32s2"]
    },
    {
      name: "v4.1-rc",
      pre_release: true
    },
    {
      name: "v4.0.1",
      old: false
    },
    {
      name: "v4.0",
      old: true
    },
    {
      name: "v3.3.2"
    },
    {
      name: "v3.3.1",
      old: true
    },
    {
      name: "v3.3",
      old: true
    },
    {
      name: "v3.2.3",
      old: false
    },
    {
      name: "v3.1.7",
      old: false
    },
    {
      name: "v3.1.6",
      old: true
    },
    {
      name: "v3.0.9",
      old: true
    },
    {
      name: "release-v4.2",
      pre_release: true,
      has_targets: true,
      supported_targets: ["esp32", "esp32s2"]
    },
    {
      name: "release-v4.1",
      pre_release: true
    },
    {
      name: "release-v4.0",
      pre_release: true
    },
    {
      name: "release-v3.3",
      pre_release: true
    },
    {
      name: "release-v3.2",
      pre_release: true
    },
    {
      name: "release-v3.1",
      pre_release: true
    }
  ],
  RELEASES: {
    "v3.1": {
      "start_date": "2018-09-07",
      "end_date": "2020-10-31"
    },
    "v3.2": {
      "start_date": "2019-04-11",
      "end_date": "2020-10-31"
    },
    "v3.3": {
      "start_date": "2019-09-05",
      "end_date": "2022-03-31"
    },
    "v4.0": {
      "start_date": "2020-02-11",
      "end_service": "2021-02-11",
      "end_date": "2021-10-31"
    }
  }
};
