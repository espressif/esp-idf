#
# Main Makefile. This is basically the same as a component makefile.
#

ifdef CONFIG_EXAMPLE_EMBEDDED_CERTS
# Certificate files. certificate.pem.crt & private.pem.key must be downloaded
# from AWS, see README for details.
COMPONENT_EMBED_TXTFILES := certs/aws-root-ca.pem certs/certificate.pem.crt certs/private.pem.key

ifndef IDF_CI_BUILD
# Print an error if the certificate/key files are missing
$(COMPONENT_PATH)/certs/certificate.pem.crt $(COMPONENT_PATH)/certs/private.pem.key:
	@echo "Missing PEM file $@. This file identifies the ESP32 to AWS for the example, see README for details."
	exit 1
else  # IDF_CI_BUILD
# this case is for the internal Continuous Integration build which
# compiles all examples. Add some dummy certs so the example can
# compile (even though it won't work)
$(COMPONENT_PATH)/certs/certificate.pem.crt $(COMPONENT_PATH)/certs/private.pem.key:
	echo "Dummy certificate data for continuous integration" > $@
endif
endif
