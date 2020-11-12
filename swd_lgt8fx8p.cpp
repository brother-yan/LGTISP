#include "swd_lgt8fx8p.h"

void SWD_init()
{
  // set to output
  SWDIF_DIR |= (SWDIF_CLK | SWDIF_DAT);
  // clear output
  SWD_SET();
  SWC_SET();
}

void SWD_exit()
{
  /* 禁止：reset后halt CPU，并lock flash */
  SWD_WriteByte(1, 0xb1, 0);
  SWD_WriteByte(0, 0x0d, 1);
  SWD_Idle(2);
  
  delayus(200);
  
  /* software reset */
  SWD_WriteByte(1, 0xb1, 0);
  SWD_WriteByte(0, 0x0c, 1);
  SWD_Idle(2);
  
  SWD_Idle(40);
  
  //SWDIF_DIR &= ~(SWDIF_CLK | SWDIF_DAT);
}

void SWD_WriteByte(uint8_t start, uint8_t data, uint8_t stop)
{
  volatile uint8_t cnt;
  
  if(start) {
    SWC_CLR();
    SWD_Delay();
    SWD_CLR();
    SWD_Delay();
    SWC_SET();
    SWD_Delay();
  }
  
  // send data
  for(cnt = 0; cnt < 8; cnt++)
  {
    SWC_CLR();
    if(data & 0x1) SWD_SET();
    else SWD_CLR();
    SWD_Delay();
    data >>= 1;
    SWC_SET();
    SWD_Delay();
  }
  
  SWC_CLR();
  if(stop) SWD_SET();
  else SWD_CLR();
  
  SWD_Delay();
  SWC_SET();
  SWD_Delay();
}

uint8_t SWD_ReadByte(uint8_t start, uint8_t stop)
{
  volatile uint8_t cnt;
  volatile uint8_t bRes = 0;
  
  if(start)
  {
    SWC_CLR();
    SWD_Delay();
    SWD_CLR();
    SWD_Delay();
    SWC_SET();
    SWD_Delay();		
  }
  
  SWD_IND();
  //SWD_Delay();
  for(cnt = 0; cnt < 8; cnt++)
  {
    bRes >>= 1;
    SWC_CLR();
    SWD_Delay();
    if(SWDIF_PIN & SWDIF_DAT)
      bRes |= 0x80;
    
    SWC_SET();
    SWD_Delay();
  }
  
  SWD_OUD();
  
  SWC_CLR();
  if(stop) SWD_SET();
  else SWD_CLR();
  
  SWD_Delay();
  SWC_SET();
  SWD_Delay();
  
  return bRes;
}

void SWD_Idle(uint8_t cnt)
{
  volatile uint8_t i;
  
  SWD_SET();
  
  for(i = 0; i < cnt; i++)
  {
    SWC_CLR();
    SWD_Delay();
    SWC_SET();
    SWD_Delay();
  }
}

void SWD_ReadSWDID(char *pdata)
{	
  SWD_WriteByte(1, 0xae, 1);
  SWD_Idle(4);
  pdata[0] = SWD_ReadByte(1, 0);
  pdata[1] = SWD_ReadByte(0, 0);
  pdata[2] = SWD_ReadByte(0, 0);
  pdata[3] = SWD_ReadByte(0, 1);
  SWD_Idle(4);
}

void SWD_ReadGUID(char *guid)
{
  SWD_Idle(10);
  SWD_WriteByte(1, 0xa8, 1);
  SWD_Idle(4);
  guid[0] = SWD_ReadByte(1, 0);
  guid[1] = SWD_ReadByte(0, 0);
  guid[2] = SWD_ReadByte(0, 0);
  guid[3] = SWD_ReadByte(0, 1);
  SWD_Idle(4);
}

void SWD_SWDEN()
{
  SWD_WriteByte(1, 0xd0, 0);
  SWD_WriteByte(0, 0xaa, 0);
  SWD_WriteByte(0, 0x55, 0);
  SWD_WriteByte(0, 0xaa, 0);
  SWD_WriteByte(0, 0x55, 1);
  SWD_Idle(4);
}

void SWD_UnLock0()
{
  SWD_WriteByte(1, 0xf0, 0);
  SWD_WriteByte(0, 0x54, 0);
  SWD_WriteByte(0, 0x51, 0);
  SWD_WriteByte(0, 0x4a, 0);
  SWD_WriteByte(0, 0x4c, 1);
  SWD_Idle(4);
}

void SWD_UnLock1()
{
  SWD_WriteByte(1, 0xf0, 0);
  SWD_WriteByte(0, 0x00, 0);
  SWD_WriteByte(0, 0x00, 0);
  SWD_WriteByte(0, 0x00, 0);
  SWD_WriteByte(0, 0x00, 1);
  SWD_Idle(4);
}

void SWD_UnLock2()
{
  SWD_WriteByte(1, 0xf0, 0);
  SWD_WriteByte(0, 0x43, 0);
  SWD_WriteByte(0, 0x40, 0);
  SWD_WriteByte(0, 0x59, 0);
  SWD_WriteByte(0, 0x5d, 1);
  SWD_Idle(4);
}

void SWD_EEE_CSEQ(uint8_t ctrl, uint16_t addr)
{
  SWD_WriteByte(1, 0xb2, 0);
  SWD_WriteByte(0, (addr & 0xff), 0);
  SWD_WriteByte(0, ((ctrl & 0x3) << 6) | ((addr >> 8) & 0x3f), 0);
  SWD_WriteByte(0, (0xC0 | (ctrl >> 2)), 1);
  SWD_Idle(4);
}

void SWD_EEE_DSEQ(uint32_t data)
{
  SWD_WriteByte(1, 0xb2, 0);
  SWD_WriteByte(0, ((uint8_t *)&data)[0], 0);
  SWD_WriteByte(0, ((uint8_t *)&data)[1], 0);
  SWD_WriteByte(0, ((uint8_t *)&data)[2], 0);
  SWD_WriteByte(0, ((uint8_t *)&data)[3], 1);
  SWD_Idle(4);
}

uint8_t SWD_EEE_GetBusy()
{
  uint8_t res = 0;
  
  SWD_WriteByte(1, 0xaa, 1);
  SWD_Idle(8);
  SWD_ReadByte(1, 0);
  SWD_ReadByte(0, 0);
  res = SWD_ReadByte(0, 1);
  SWD_Idle(8);
  
  return res & 0x1;
}

void SWD_ChipErase()
{
  /* reset flash to 0xff */
  // 我猜是操作FLASH访问控制寄存器 - EECR
  SWD_EEE_CSEQ(0x00, 1);
  SWD_EEE_CSEQ(0x98, 1);
  SWD_EEE_CSEQ(0x9a, 1);
  delay(200);
  SWD_EEE_CSEQ(0x8a, 1);
  delay(20);
  SWD_EEE_CSEQ(0x88, 1);
  SWD_EEE_CSEQ(0x00, 1);
}

void crack() // 破解读保护(目前只能读出除了前1k以外的flash，前1k会被擦除)
{
  SWD_EEE_CSEQ(0x00, 1);
  SWD_EEE_CSEQ(0x98, 1);
  SWD_EEE_CSEQ(0x92, 1); // 会擦除flash的第一页(1024 bytes)
  delay(200);
  SWD_EEE_CSEQ(0x9e, 1); // 解锁
  delay(200);
  SWD_EEE_CSEQ(0x8a, 1);
  delay(20);
  SWD_EEE_CSEQ(0x88, 1);
  SWD_EEE_CSEQ(0x00, 1);
}

uint32_t SWD_EEE_Read(uint16_t addr)
{
  uint32_t data;
  
  SWD_EEE_CSEQ(0xc0, addr);
  SWD_EEE_CSEQ(0xe0, addr);
  
  SWD_WriteByte(1, 0xaa, 1);
  ((uint8_t *)&data)[0] = SWD_ReadByte(1, 0);
  ((uint8_t *)&data)[1] = SWD_ReadByte(0, 0);
  ((uint8_t *)&data)[2] = SWD_ReadByte(0, 0);
  ((uint8_t *)&data)[3] = SWD_ReadByte(0, 1);
  SWD_Idle(4);
  
  return data;
}

void SWD_EEE_Write(uint32_t data, uint16_t addr)
{
  SWD_EEE_DSEQ(data);
  SWD_EEE_CSEQ(0x86, addr);
  SWD_EEE_CSEQ(0xc6, addr);
  SWD_EEE_CSEQ(0x86, addr);
}

uint8_t SWD_UnLock(uint8_t chip_erase)
{
  char swdid[4];
  
  SWD_ReadSWDID(swdid); // {0x3e, 0xa2, 0x50, 0xe9}表示这是第一次进行SWD操作，{0x3f, 0xa2, 0x50, 0xe9}表示之前进行过SWD解锁操作
  SWD_SWDEN();
  
  if (! (swdid[0] == 0x3e || swdid[0] == 0x3f)) // invalid device
    return 0;
  
  if (swdid[0] == 0x3f && !chip_erase) // 已经解锁，且不全片擦除
    return 1;
  
  if (swdid[0] == 0x3e) // 第一次解锁
    SWD_UnLock0();
  
  if (chip_erase)
    SWD_ChipErase();
  else
    crack();
  
  if (swdid[0] == 0x3e) // 第一次解锁
    {
      SWD_UnLock1();
      // 此时swdid[0] == 0x3f
      
      SWD_WriteByte(1, 0xb1, 0);
      SWD_WriteByte(0, 0x3d, 0);
      SWD_WriteByte(0, 0x60, 0);
      SWD_WriteByte(0, 0x0c, 0);
      SWD_WriteByte(0, 0x00, 0);
      SWD_WriteByte(0, 0x0f, 1);
      SWD_Idle(40);
      
      SWD_UnLock2();
    }
  SWD_Idle(40);
  
  SWD_WriteByte(1, 0xb1, 0);
  SWD_WriteByte(0, 0x0c, 0);
  SWD_WriteByte(0, 0x00, 0);
  SWD_WriteByte(0, 0x17, 1);
  SWD_Idle(40);
  
  char flag[2];
  SWD_WriteByte(1, 0xa9, 1);
  SWD_Idle(4);
  flag[0] = SWD_ReadByte(1, 0);
  flag[1] = SWD_ReadByte(0, 1);
  SWD_Idle(4);
  
  if (flag[1] == 0x20) // 0x60没有这段命令
    {
      SWD_WriteByte(1, 0xb1, 0);
      SWD_WriteByte(0, 0x3d, 0);
      SWD_WriteByte(0, 0x20, 0);
      SWD_WriteByte(0, 0x0c, 0);
      SWD_WriteByte(0, 0x00, 0);
      SWD_WriteByte(0, 0x0f, 1);
      SWD_Idle(40);
    }
  else if (flag[1] == 0x60) // 不接reset引脚会返回这个
    {
    }
  else
    return 0;
  
  SWD_WriteByte(1, 0xb1, 0);
  SWD_WriteByte(0, 0x0d, 1);
  SWD_Idle(2);
  
  return 1;
}

void write_flash_pages(uint32_t addr, uint8_t buf[], int size)
{
  addr /= 4; // lgt8fx8p的flash是按4字节编址的，而传入的参数是字节地址
  
  SWD_EEE_CSEQ(0x00, addr);
  SWD_EEE_CSEQ(0x84, addr);
  SWD_EEE_CSEQ(0x86, addr);
  
  for (int i = 0; i < size; i += 4)
    {
      SWD_EEE_Write(*((uint32_t *)(&buf[i])), addr);
      ++addr;
    }
  
  SWD_EEE_CSEQ(0x82, addr - 1);
  SWD_EEE_CSEQ(0x80, addr - 1);
  SWD_EEE_CSEQ(0x00, addr - 1);
}

void flash_read_page(uint32_t addr, uint8_t buf[], int size)
{
  addr /= 4; // lgt8fx8p的flash是按4字节编址的，而传入的参数是字节地址
  
  SWD_EEE_CSEQ(0x00, 0x01);
  
  uint32_t data;
  for (int i = 0; i < size; ++i)
    {
      if (i % 4 == 0)
        {
          data = SWD_EEE_Read(addr);
          ++addr;
        }
      buf[i] = ((uint8_t *)&data)[i % 4];
    }
  
  SWD_EEE_CSEQ(0x00, 0x01);
}

volatile uint8_t pmode = 0;
void start_pmode(uint8_t chip_erase)
{
  RSTN_SET(); // digitalWrite(RESET, HIGH);
  RSTN_OUD(); // pinMode(RESET, OUTPUT);
  delay(20);
  RSTN_CLR(); // digitalWrite(RESET, LOW);
  
  SWD_init();
  SWD_Idle(80);
  
  pmode = SWD_UnLock(chip_erase);
  if (!pmode)
    pmode = SWD_UnLock(chip_erase);
}

void end_pmode()
{
  SWD_exit();
  pmode = 0;
  
  RSTN_SET(); // digitalWrite(RESET, HIGH);
  RSTN_IND(); // pinMode(RESET, INPUT);
}




LGTISPClass::LGTISPClass()
{
  SWD_init();
  chip_erased = 0;
}

bool LGTISPClass::begin()
{
  if (!pmode)
    {
      start_pmode(0);
      chip_erased = 0;
    }
  
  SWD_ReadGUID(guid);
  
  return pmode;
}

void LGTISPClass::end()
{
  end_pmode();
}

bool LGTISPClass::write(uint32_t addr, uint8_t buf[], int size)
{
  if (!chip_erased)
      {
        end_pmode();
        start_pmode(1);
        chip_erased = 1;
      }
  
  if (pmode)
    write_flash_pages(addr, buf, size);
  
  return pmode;
}

bool LGTISPClass::read(uint32_t addr, uint8_t buf[], int size)
{
  if (pmode)
    flash_read_page(addr, buf, size);
  
  return pmode;
}

bool LGTISPClass::isPmode()
{
  return pmode;
}

uint32_t LGTISPClass::getGUID() // return a 4 bytes guid
{
  SWD_ReadGUID(guid);
  
  return *((uint32_t *)guid);
}

LGTISPClass LGTISP;
