
import base64
import json
import collections
import time
import datetime
import sys

class LogConverter:

    def __init__(self):
        self.HEADER_BYTE = 204
        self.HEADER_NIBBLE = 192
        self.TYPE_MASK = 7
        self.MESSAGE_LENGTH = 10

        self._data = {}
        self._components = {}

    def sortDict(self, od):
        res = collections.OrderedDict()
        for k, v in sorted(od.items(), key=lambda t: t[0]):
            if isinstance(v, dict):
                res[k] = self.sortDict(v)
            else:
                res[k] = v
        return res

    def LoadJson(self, filename):

        self._data = json.load(open(filename))
        # sort dictionary alphabetically to match the message indices
        self._components = self.sortDict(self._data['log']['components'])
        #print(json.dumps(self._components, indent=4))

    def ParseEncoded(self, encodedLog):

        # decode into integer array
        encodedLog = str(encodedLog).strip() + "==="
        try:
            decoded = [ord(c) for c in encodedLog.decode('base64')]
        except __import__('binascii').Error as err:
            print "Unexpected error:", err
            return []

        # split decoded string into message array
        messages = []

        if len(decoded) > 0:
            for i in range(0, len(decoded) - self.MESSAGE_LENGTH + 1):
                if decoded[i] == self.HEADER_BYTE and (decoded[i+1] & self.HEADER_NIBBLE) == self.HEADER_NIBBLE:
                    message = {}
                    message['type'] = decoded[i+1] & self.TYPE_MASK
                    message['line'] = decoded[i+3] << 8 | decoded[i+2]
                    message['time'] = decoded[i+7] << 24 | decoded[i+6] << 16 | decoded[i+5] << 8 | decoded[i+4]
                    message['timestamp'] = datetime.datetime.utcfromtimestamp(message['time']).isoformat()
                    message['timeText'] = time.strftime('%b %d %Y %H:%M:%S', time.localtime(message['time']))
                    message['component'] = decoded[i+8]
                    message['code'] = decoded[i+9]
                    # TODO: these types shouldn't be hardcoded
                    # 1 = uint8, 2 = uint32, 3 = uint64, 4 = string
                    message['value'] = ''

                    if message['type'] == 1:
                        message['value'] = decoded[i+10]
                    elif message['type'] == 2:
                        message['value'] = decoded[i+13] << 24 | decoded[i+12] << 16 | decoded[i+11] << 8 | decoded[i+10]
                    elif message['type'] == 3:
                        message['value'] = decoded[i+17] << 56 | decoded[i+16] << 48 | decoded[i+15] << 40 | decoded[i+14] << 32 | decoded[i+13] << 24 | decoded[i+12] << 16 | decoded[i+11] << 8 | decoded[i+10]
                    elif message['type'] == 4:
                        stringLength = decoded[i+10]
                        for i in range(11, 11 + stringLength):
                            message['value'] += chr(decoded[i])

                    self.MessageLookup(message)
                    messages.append(message)

        return messages

    def MessageLookup(self, message):
        message['componentText'] = self._components.keys()[message['component']]
        message['codeText'] = 'LOG ERROR: LOOKUP FAILED'

        i = 0
        for level in ['critical', 'debug', 'error', 'info', 'warning']:
            if i + len(self._components[message['componentText']][level].keys()) > message['code']:
                message['codeText'] = self._components[message['componentText']][level].values()[message['code'] - i]
                message['codeText'] = message['codeText'].replace('{uint8}', str(message['value']))
                message['codeText'] = message['codeText'].replace('{int32}', str(message['value']))
                message['codeText'] = message['codeText'].replace('{int64}', str(message['value']))
                message['codeText'] = message['codeText'].replace('{string}', str(message['value']))
                message['level'] = level
                break
            else:
                i += len(self._components[message['componentText']][level].keys())

    def FormatMessage(self, message):
        return message['timeText'] + ' | ' + message['level'].upper().ljust(8) + " | " + (message['componentText'].title() + "." + str(message['line'])).ljust(14) + ' | ' + message['codeText']

if __name__ == "__main__":
    logTest = LogConverter()
    logTest.LoadJson('log_text.json')
    logTest.ParseEncoded("zMBhAA0AAAADAMzAPwAVAAAAAQHMwD8AFwAAAAEBzMBFACERtVoBAMzARQAjEbVaAQDMwP4AJhG1WgMBzMA/ACcRtVoBAczARQAnEbVaAQDMwD8AKBG1WgEBzMBFACoRtVoBAMzAPwAsEbVaAQHMwEUALBG1WgEAzMA/AC8RtVoBAczARQAvEbVaAQDMwD8AMhG1WgEBzMBFADIRtVoBAMzA/gA1EbVaAwHMwD8ANxG1WgEBzMBFADcRtVoBAMzAPwA9EbVaAQHMwEUAPRG1WgEAzMA/AD4RtVoBAQA=")

