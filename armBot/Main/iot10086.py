import requests
import json
class Iot10086(object):
    apikey='p2CLc6GlMAsKnBwSQ80H3TeQMec='
    apiurl='http://api.heclouds.com/devices/710289579/datastreams/'

    def __init__(self, apikey, apiurl):
        self.apikey = apikey
        self.apiurl = apiurl
        self.apiheaders={'api-key':apikey}

    def set_data(self,datastream,value):
        apiurl_set = self.apiurl + '?type=3'
        payload={datastream:str(value)}
        r=requests.post(apiurl_set, headers=self.apiheaders, data=json.dumps(payload), timeout=30)
        return r.status_code

    def get_data(self,datastream):
        apiurl_get = self.apiurl + datastream
        r=requests.get(apiurl_get, headers=self.apiheaders, timeout=20)
        if r.status_code == 200:
            return json.loads(r.text)
        else:
            return r.status_code
