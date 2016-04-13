#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "sim900a.h"



//0表示成功，1表示失败

uint8_t sim900a_cmd(char *cmd, char *reply,uint32_t waittime )
{    
    SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据

    SIM900A_TX(cmd);                    //发送命令

    if(reply == 0)                      //不需要接收数据
    {
        return SIM900A_TURE;
    }
    
    SIM900A_DELAY(waittime);                 //延时
    
    
    return sim900a_cmd_check(reply);    //对接收数据进行处理
}


//0表示成功，1表示失败
uint8_t sim900a_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;
    
    redata = SIM900A_RX(len);   //接收数据
    
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //数据为空或者比较完毕
        {
            return SIM900A_TURE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                //移动到下一个接收数据
        }
        else
        {
            off++;              //进行下一轮匹配
            n=0;                //重来
        }
        //n++;
    }

    if(reply[n]==0)   //刚好匹配完毕
    {
        return SIM900A_TURE;
    }
    
    return SIM900A_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

char * sim900a_waitask(uint8_t waitask_hook(void))      //等待有数据应答
{
    uint8_t len;
    char *redata;
    do{
        redata = SIM900A_RX(len);   //接收数据
        if(waitask_hook!=0)
        {
            if(waitask_hook()==SIM900A_TURE)           //返回 SIM900A_TURE 表示检测到事件，需要退出
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //接收数据为0时一直等待
    
    
    SIM900A_DELAY(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
    return redata;
}


//本机号码
//0表示成功，1表示失败
uint8_t sim900a_cnum(char *num)
{
    char *redata;
    uint8_t len;
    
    if(sim900a_cmd("AT+CNUM\r","OK", 100) != SIM900A_TURE)
    {
        return SIM900A_FALSE;
    }
    
    redata = SIM900A_RX(len);   //接收数据
    
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    //第一个逗号后面的数据为:"本机号码"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return SIM900A_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
    return SIM900A_TURE;
}


//初始化并检测模块
//0表示成功，1表示失败
uint8_t sim900a_init(void)
{
	char *redata;
  uint8_t len;
	
	sim900a_config();					//初始化串口
    
	if(sim900a_cmd("AT+CGMM\r","OK", 100) != SIM900A_TURE)
	{
			return SIM900A_FALSE;
	}
	
	redata = SIM900A_RX(len);   //接收数据
	
	if(len == 0)
	{
			return SIM900A_FALSE;
	}
	if (strstr(redata,"SIMCOM_SIM900A") != 0)
		return SIM900A_TURE;
	else
		return SIM900A_FALSE;

}



//发起拨打电话（不管接不接通）
void sim900a_call(char *num)
{
    SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
    
    sim900a_tx_printf("ATD%s;\r",num);

    //拨打后是不返回数据的
    //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
    //对方挂掉电话（没接通），返回：BUSY
    //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
    //对方接听了电话,然后挂掉：NO CARRIER
    //对方超时没接电话：NO ANSWER
}

uint8_t IsASSIC(char * str)
{
    while(*str)
    {
        if(*str>0x7F)
        {
            return SIM900A_FALSE;
        }
        str++;
    }
    
    return SIM900A_TURE;
}



void sim900a_gbk2ucs2(char * ucs2,char * gbk)
{
    WCHAR   tmp;
    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //英文
        {
            
            *ucs2++ = 0;
            *ucs2++ = *gbk++;  
        }
        else                        //中文
        {
            tmp = SIM900A_SWAP16(*(WCHAR *)gbk);
            
            *(WCHAR *)ucs2 = SIM900A_SWAP16(ff_convert(tmp,1));
            gbk+=2;
            ucs2+=2;
        }
    }
}

void sim900a_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    *hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}

void sim900a_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
    WCHAR   tmp;
    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //英文
        {
            
            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            sim900a_char2hex(ucs2hex,*gbk);
            ucs2hex+=2;
            gbk++;  
        }
        else                        //中文
        {
            tmp = SIM900A_SWAP16(*(WCHAR *)gbk);
            tmp = ff_convert(tmp,1);
            sim900a_char2hex(ucs2hex,(char)(tmp>>8));
            ucs2hex+=2;
            sim900a_char2hex(ucs2hex,(char)tmp);
            ucs2hex+=2;
            gbk+=2;
        }
    }
    *ucs2hex=0;
}



//发送短信（支持中英文,中文为GBK码）
void sim900a_sms(char *num,char *smstext)
{
    char ucsbuff[160];
	  char end[2] = {0x1A,0x00};

    SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
	
    if(IsASSIC(smstext)==SIM900A_TURE)
    {
        //英文
        sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",num);  //电话号码
        SIM900A_DELAY(100);

        sim900a_tx_printf("%s",smstext);            //短信内容
        //SIM900A_DELAY(100);          
    }
    else
    {
        //中文
        sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,num);
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2的电话号码(需要转成 ucs2码)
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,smstext);
        sim900a_tx_printf("%s\r",ucsbuff);          //UCS2的文本内容(需要转成 ucs2码)
        //SIM900A_DELAY(100);
    }
    
    sim900a_tx_printf("%s",end);
}



//发送短信（支持中英文,输入时使用UTF8编码）
void sim900a_sms_utf8(char *num,char *smstext,uint16_t numlen,uint16_t textlen)
{
    char ucsbuff[160];
	  char end[2] = {0x1A,0x00};

    SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
	
    if(IsASSIC(smstext)==SIM900A_TURE)
    {
        //英文
        sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",num);  //电话号码
        SIM900A_DELAY(100);

        sim900a_tx_printf("%s",smstext);            //短信内容
        //SIM900A_DELAY(100);          
    }
    else
    {
        //中文
        sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
//        sim900a_utf82ucs2hex(ucsbuff,num,numlen);
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2的电话号码(需要转成 ucs2码)
        SIM900A_DELAY(100);

//        sim900a_utf82ucs2hex(ucsbuff,smstext,textlen);
        sim900a_tx_printf("%s\r",ucsbuff);          //UCS2的文本内容(需要转成 ucs2码)
        //SIM900A_DELAY(100);			

    }
    
    sim900a_tx_printf("%s",end);
}



void sim900a_gprs_init(void)
{
    sim900a_tx_printf("AT+CGCLASS=\"B\"\r");                       //设置移动台类别为"B"
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");          //PDP上下文标识1，互联网协议，接入点名称：CMNET
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CGATT=1\r");                             //附着 GPRS 网络
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CIPCSGP=1,\"CMNET\"\r");                 //设置为 GPRS 连接模式 ，接入点为CMNET
}

void sim900a_gprs_tcp_link(char *localport,char * serverip,char * serverport)
{
    sim900a_tx_printf("AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //获取本地端口
    SIM900A_DELAY(100);

    //设置服务器IP和端口
    sim900a_tx_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
}

void sim900a_gprs_udp_link(char *localport,char * serverip,char * serverport)
{
    sim900a_tx_printf("AT+CLPORT=\"UDP\",\"%s\"\r",localport);              //获取本地端口
    SIM900A_DELAY(100);

    sim900a_tx_printf("AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);   //设置服务器IP和端口
}

void sim900a_gprs_send(char * str)
{
    sim900a_tx_printf("AT+CIPSEND\r");
    SIM900A_DELAY(100);
        
    sim900a_tx_printf("%s%c",str,0x1A);
}

void sim900a_gprs_link_close(void)               //IP链接断开
{
    sim900a_tx_printf("AT+CIPCLOSE=1\r");
}
