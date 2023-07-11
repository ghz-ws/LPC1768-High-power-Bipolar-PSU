#include "mbed.h"

BufferedSerial uart0(P0_0, P0_1,115200);  //TX, RX
I2C i2c(P0_10,P0_11);
I2C i2c2(P0_27,P0_28);
SPI spi(P0_9,P0_8,P0_7);        //ch1&2 DAC
DigitalOut cs1(P0_6);           //DAC1 cs
DigitalOut cs2(P0_5);           //DAC2 cs
SPI spi2(P0_18,P0_17,P0_15);    //ch3&4 DAC
DigitalOut cs3(P0_19);          //DAC3 cs
DigitalOut cs4(P0_20);          //DAC4 cs

AnalogIn a0(P0_23);     //VM1
AnalogIn a1(P0_24);     //IM1
DigitalOut io0(P2_0);   //EN1. on=1, off=0
DigitalIn io1(P2_1);    //TSD1
DigitalIn io2(P2_2);    //ISNK/SRC1

AnalogIn a2(P0_25);     //VM2
AnalogIn a3(P0_26);     //IM2
DigitalOut io3(P2_3);   //EN2. on=1, off=0
DigitalIn io4(P2_4);    //TSD2
DigitalIn io5(P2_5);    //ISNK/SRC2

AnalogIn a4(P1_30);     //VM3
AnalogIn a5(P1_31);     //IM3
DigitalOut io6(P2_6);   //EN3. on=1, off=0
DigitalIn io7(P2_7);    //TSD3
DigitalIn io8(P2_8);    //ISNK/SRC3

AnalogIn a6(P0_3);     //VM4
AnalogIn a7(P0_2);     //IM4
DigitalOut io9(P1_9);   //EN4. on=1, off=0
DigitalIn io10(P1_10);    //TSD4
DigitalIn io11(P1_14);    //ISNK/SRC4

//uart read&send
const uint8_t buf_size=6+3+1;
void buf_read(uint8_t num);                 //uart read func.
char read_buf[buf_size];                    //uart read buf
void buf2val();                             //buf to vals change func. return to 'vs' and 'is' and 'en' global var 
void val_send(uint8_t digit, int16_t val);  //uart send func.
void err_send(uint8_t val);                 //uart err send func.
int16_t vs;     //mV
uint16_t is;    //mA
uint8_t en;     //on=1, off=0

//DAC settings
#define g 3     //LT1970A dif amp gain
#define R 0.5   //sens R
#define res 4   //4.096/2^10 dac resolution
uint16_t calc_a(uint16_t is);
uint16_t calc_b(uint16_t is);
uint16_t calc_c(int16_t vs);
uint16_t calc_d(int16_t vs);
int16_t vs_check(int16_t vs);
uint16_t is_check(uint16_t is);
int16_t vs1,vs2,vs3,vs4;    //mV unit
uint16_t is1,is2,is3,is4;   //mA unit  

//meas settings
#define vc 1800 //center  voltage
#define att 0.1 //vm attenuation
#define g_c 3   //LT1991 dif amp gain
const uint8_t avg=200;
uint8_t cnt;
float vm1_all,im1_all,vm2_all,im2_all,vm3_all,im3_all,vm4_all,im4_all;
int16_t vm1,im1,vm2,im2,vm3,im3,vm4,im4;

int main(){
    spi.format(16,0);   //spi mode setting. 2byte(16bit) transfer, mode 0
    spi2.format(16,0);
    cs1=1;
    cs2=1;
    cs3=1;
    cs4=1;
    io0=0;
    io3=0;
    io6=0;
    io9=0;
    while (true){
        //ch1
        buf_read(buf_size); //uart buf read
        buf2val();          //buf to val
        if(en==0){          //en off
            vs1=0;
            is1=0;
            io0=0;
        }else if(en==1){    //en on
            io0=1;
            vs1=vs_check(vs);
            is1=is_check(is);
        }
        cs1=0;
        spi.write(calc_a(is1));
        cs1=1;
        cs1=0;
        spi.write(calc_b(is1));
        cs1=1;
        cs1=0;
        spi.write(calc_c(vs1));
        cs1=1;
        cs1=0;
        spi.write(calc_d(vs1));
        cs1=1;

        //ch2
        buf_read(buf_size); //uart buf read
        buf2val();          //buf to val
        if(en==0){          //en off
            vs2=0;
            is2=0;
            io3=0;
        }else if(en==1){    //en on
            io3=1;
            vs2=vs_check(vs);
            is2=is_check(is);
        }
        cs2=0;
        spi.write(calc_a(is2));
        cs2=1;
        cs2=0;
        spi.write(calc_b(is2));
        cs2=1;
        cs2=0;
        spi.write(calc_c(vs2));
        cs2=1;
        cs2=0;
        spi.write(calc_d(vs2));
        cs2=1;

        //ch3
        buf_read(buf_size); //uart buf read
        buf2val();          //buf to val
        if(en==0){          //en off
            vs3=0;
            is3=0;
            io6=0;
        }else if(en==1){    //en on
            io6=1;
            vs3=vs_check(vs);
            is3=is_check(is);
        }
        cs3=0;
        spi2.write(calc_a(is3));
        cs3=1;
        cs3=0;
        spi2.write(calc_b(is3));
        cs3=1;
        cs3=0;
        spi2.write(calc_c(vs3));
        cs3=1;
        cs3=0;
        spi2.write(calc_d(vs3));
        cs3=1;

        //ch4
        buf_read(buf_size); //uart buf read
        buf2val();          //buf to val
        if(en==0){          //en off
            vs4=0;
            is4=0;
            io9=0;
        }else if(en==1){    //en on
            io9=1;
            vs4=vs_check(vs);
            is4=is_check(is);
        }
        cs4=0;
        spi2.write(calc_a(is4));
        cs4=1;
        cs4=0;
        spi2.write(calc_b(is4));
        cs4=1;
        cs4=0;
        spi2.write(calc_c(vs4));
        cs4=1;
        cs4=0;
        spi2.write(calc_d(vs4));
        cs4=1;

        //meas
        vm1_all=0;
        im1_all=0;
        vm2_all=0;
        im2_all=0;
        vm3_all=0;
        im3_all=0;
        vm4_all=0;
        im4_all=0;
        for(cnt=0;cnt<avg;++cnt){
            //ch1 meas
            vm1_all=vm1_all+((a0.read()*3300)-vc)/att+vc;
            im1_all=im1_all+((a1.read()*3300)-vc)/R/g_c*-1;
            //ch2 meas
            vm2_all=vm2_all+((a2.read()*3300)-vc)/att+vc;
            im2_all=im2_all+((a3.read()*3300)-vc)/R/g_c*-1;
            //ch3 meas
            vm3_all=vm3_all+((a4.read()*3300)-vc)/att+vc;
            im3_all=im3_all+((a5.read()*3300)-vc)/R/g_c*-1;
            //ch4 meas
            vm4_all=vm4_all+((a6.read()*3300)-vc)/att+vc;
            im4_all=im4_all+((a7.read()*3300)-vc)/R/g_c*-1;
        }
        vm1=(int16_t)vm1_all/avg;
        im1=(int16_t)im1_all/avg;
        vm2=(int16_t)vm2_all/avg;
        im2=(int16_t)im2_all/avg;
        vm3=(int16_t)vm3_all/avg;
        im3=(int16_t)im3_all/avg;
        vm4=(int16_t)vm4_all/avg;
        im4=(int16_t)im4_all/avg;
        
        val_send(5,vm1);
        val_send(3,im1);
        val_send(5,vm2);
        val_send(3,im2);
        val_send(5,vm3);
        val_send(3,im3);
        val_send(5,vm4);
        val_send(3,im4);

        //error status
        //ch1
        if(io1==0) err_send(1);    //TSD1
        else err_send(0);
        if(io2==0) err_send(1);  //ISNK/SRC1
        else err_send(0);
        //ch2
        if(io4==0) err_send(1);  //TSD2
        else err_send(0);
        if(io5==0) err_send(1);  //ISNK/SRC2
        else err_send(0);
        //ch3
        if(io7==0) err_send(1);  //TSD3
        else err_send(0);
        if(io8==0) err_send(1);  //ISNK/SRC3
        else err_send(0);
        //ch4
        if(io10==0) err_send(1);  //TSD4
        else err_send(0);
        if(io11==0) err_send(1);  //ISNK/SRC4
        else err_send(0);
    }
}

//uart char number read func.
void buf_read(uint8_t num){
    char local_buf[1];
    uint8_t i;
    for (i=0;i<num;++i){
        uart0.read(local_buf,1);
        read_buf[i]=local_buf[0];
    }
}

//buf to val change func.
void buf2val(){
    uint8_t i,j;
    uint32_t pow10;
    vs=0;
    is=0;
    en=0;
    for(i=0;i<5;++i){
        pow10=1;
        for(j=0;j<4-i;++j){
            pow10=10*pow10;
        }
        vs=vs+(read_buf[i+1]-48)*pow10;
    }
    if(read_buf[0]==43)vs=vs;
    else if(read_buf[0]==45)vs=-1*vs;
    for(i=0;i<3;++i){
        pow10=1;
        for(j=0;j<2-i;++j){
            pow10=10*pow10;
        }
        is=is+(read_buf[i+6]-48)*pow10;
    }
    if(read_buf[9]==49)en=1;
    else if(read_buf[9]==48)en=0;
}

//vs val check func.
int16_t vs_check(int16_t vs){
    int16_t out;
    if(vs>15000)out=15000;
    else if(vs<-15000)out=-15000;
    else out=vs;
    return out;
}

//is val check func.
uint16_t is_check(uint16_t is){
    int16_t out;
    if(is>810)out=810;
    else out=is;
    return out;
}

//uart send func.
void val_send(uint8_t digit, int16_t val){
    char local_buf[1];
    char data[5];
    uint8_t i;
    if(val<0){
        val=abs(val);
        local_buf[0]=45;
        uart0.write(local_buf,1);   //send minus
    }else{
        val=val;
        local_buf[0]=43;
        uart0.write(local_buf,1);   //send plus
    }
    data[4]=0x30+val%10;            //1
    data[3]=0x30+(val/10)%10;       //10
    data[2]=0x30+(val/100)%10;      //100
    data[1]=0x30+(val/1000)%10;     //1000
    data[0]=0x30+(val/10000)%10;    //10000
    for(i=0;i<digit;++i){
        local_buf[0]=data[i+5-digit];
        uart0.write(local_buf,1);
    }
}

//uart err send.
void err_send(uint8_t val){
    char local_buf[1];
    local_buf[0]=0x30+val;
    uart0.write(local_buf,1);
}

//DAC reg calc func.
uint16_t calc_a(uint16_t is){
    uint16_t a_val;
    a_val=(is*10*R)/res;            //ilim dac val
    return (0b0001<<12)+(a_val<<2); //daca=0b0001
}
uint16_t calc_b(uint16_t is){
    uint16_t a_val;
    a_val=(is*10*R)/res;
    return (0b0010<<12)+(a_val<<2); //dacb=0b0010
}
uint16_t calc_c(int16_t vs){
    uint16_t c_val;
    if(vs<0) c_val=0;
    else c_val=(uint16_t)abs(vs)/g/res;
    return (0b0011<<12)+(c_val<<2); //dacc=0b0011
}
uint16_t calc_d(int16_t vs){
    uint16_t d_val;
    if(vs<0) d_val=(uint16_t)abs(vs)/g/res;
    else d_val=0;
    return (0b0100<<12)+(d_val<<2); //dacd=0b0100
}