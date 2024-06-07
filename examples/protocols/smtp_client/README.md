| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# SMTP Client Example

The Example is SMTP client demo. It sends and email with attachment to recipient.


## How to use the example


### Configure the project

```
idf.py menuconfig
```

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../README.md) for more details.

Please set the following parameters under example config for SMTP client demo to work:
  - Email server, port, sender's email ID, password, recipient's email ID.


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.



## Example output

```
I (3212) smtp_example: Loading the CA root certificate...
I (3212) smtp_example: Setting hostname for TLS session...
I (3222) smtp_example: Setting up the SSL/TLS structure...
I (3232) smtp_example: Connecting to smtp.googlemail.com:587...
I (3542) smtp_example: Connected.

220 smtp.googlemail.com ESMTP r62sm20390571pfc.89 - gsmtp
I (3952) smtp_example: Writing EHLO to server...

250-smtp.googlemail.com at your service, [182.75.158.118]
250-SIZE 35882577
250-8BITMIME
250-STARTTLS
250-ENHANCEDSTATUSCOD
ES
250-PIPELINING
250-CHUNKING
250 SMTPUTF8
I (4262) smtp_example: Writing STARTTLS to server...

220 2.0.0 Ready to start TLS
I (4562) smtp_example: Performing the SSL/TLS handshake...
I (5692) smtp_example: Verifying peer X.509 certificate...
I (5692) smtp_example: Certificate verified.
I (5692) smtp_example: Cipher suite is TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256
I (5702) smtp_example: Authentication...
I (5702) smtp_example: Write AUTH LOGIN
I (6002) smtp_example: Write USER NAME
I (6302) smtp_example: Write PASSWORD
I (6822) smtp_example: Write MAIL FROM
I (7132) smtp_example: Write RCPT
I (7432) smtp_example: Write DATA
I (8252) smtp_example: Write Content
I (10202) smtp_example: Email sent!
```


## Note:
  - You might need to enable permission for less secure apps from email provider. This is because these email providers (e.g. gmail) insist on OAUTH authorization.
