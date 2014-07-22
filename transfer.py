#! /usr/bin/env python
#-*-coding:utf-8-*-

import MySQLdb
import psutil
import urllib
import time
import sys
import os

#########################################################################################################################
## MySQLdb : 在部署前需要确定系统安装了该python模块
## psutil : 在python中进行系统进程管理的模块
#########################################################################################################################

(TRANSFERSERVERCOMMAND, TRANSFERCLIENTCOMMAND, CMSSERVER, TMPDIR) = ("./transferserver", "./transferclient", 'http://cms.legaldaily.dev', './tmp', )

def serverwatchdog (): # 
    """ 内容接受服务器端监控程序：保证服务器端程序接受正常运行；查看系统传输监听进程是否在运行，若没有运行，启动监听进程 """
    checkdirectory()
    while True:
        found = False
        for process in psutil.get_process_list():
            processname = process.name()
            if processname==TRANSFERSERVERCOMMAND:
                found = True
                break
        if not found:
            os.system ( TRANSFERSERVERCOMMAND+' &' )
        time.sleep(5)

def clientwatchdog ():
    """ 内容发送程序监控进程：保证内容发送程序的正常运行；启动内容发送程序并监控内容发送程序是否运行，若没有运行，启动发送程序 """
    checkdirectory ()
    while True:
        transfertask ()
        time.sleep(5)    

def checkdirectory ():
    """启动服务器端监听任务进程，如果当前目录不在适当的目录下，则给出错误提示并退出程序"""
    if not os.path.isfile ( TRANSFERSERVERCOMMAND ):
        print "transfer server command not compiled in current directory or run command not in current directory"
        exit(0)

def transfertask ():
    """在发送的客户端运行传输任务，将需要发送的文件保存在指定的远程机器的指定位置"""
    try:
        if not os.path.isdir (TMPDIR):
            os.mkdir(TMPDIR)

        conn = MySQLdb.connect ( host='168.0.0.98', user='username', passwd='password', port=3306 )
        cur  = conn.cursor()
         
        conn.select_db('cms')
        cur.execute(' SELECT `uuid` FROM `cms_content_publish` WHERE `status`=1 ')
        cur.scroll ( 0, mode='absolute' )
     
        results = cur.fetchall()
        for r in results:
            cur.execute (' SELECT * FROM `cms_content_publish` WHERE `uuid`= %s ' % r[0] )
            cur.scroll ( 0, mode='absolute' )
            publish    = cur.fetchone()
            
            localurl   = CMSSERVER+publish[12]
            publicurl  = '/var/www/cms/public'+publish[11]
            serverhost = publish[8]
            serverport = publish[9]
            serverpswd = publish[10]
            
            filename = TMPDIR + '/' + os.path.basename ( publicurl )

            socket = urllib.urlopen(localurl)
            fout   = file ( filename, 'w' )
            fout.write ( socket.read() )
            transfercommand = "{0} {1} {2} {3} {4} {5}".format(TRANSFERCLIENTCOMMAND, filename, publicurl, serverhost, serverport, serverpswd)
            deletecommand = 'DELETE FROM `cms_content_publish` WHERE `uuid`= {0} '.format ( r[0] )
            # print transfercommand
            # print deletecommand
            os.system ( transfercommand ) # translate file to public server
            #cur.execute ( deletecommand )
            os.unlink ( filename ) # delete file that fetched from server
        
        conn.commit()
        cur.close()
        conn.close()
     
    except MySQLdb.Error,e:
         print "Mysql Error %d: %s" % (e.args[0], e.args[1])
         exit ()

if __name__=="__main__":
    argc = len ( sys.argv )
    if argc>1 :
        clientwatchdog () # 客户端守护进程
    else :
        serverwatchdog () # 服务器端守护进程
