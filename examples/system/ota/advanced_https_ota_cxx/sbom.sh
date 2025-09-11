#!/bin/bash

esp-idf-sbom create -o sbom.spdx build/project_description.json
esp-idf-sbom create --rem-unused --rem-config -o sbom-min.spdx build/project_description.json
esp-idf-sbom check sbom.spdx
