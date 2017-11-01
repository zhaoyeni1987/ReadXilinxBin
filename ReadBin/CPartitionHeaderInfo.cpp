#include "CPartitionHeaderInfo.h"

#define PARTITION_NUMBER_SHIFT	24
#define MAX_PARTITION_NUMBER	(0xE)

/* Boot Image Header defines */
#define IMAGE_HDR_OFFSET			0x098	/* Start of image header table */
#define IMAGE_PHDR_OFFSET			0x09C	/* Start of partition headers */
#define IMAGE_HEADER_SIZE			(64)
#define IMAGE_HEADER_TABLE_SIZE		(64)
#define TOTAL_PARTITION_HEADER_SIZE	(MAX_PARTITION_NUMBER * IMAGE_HEADER_SIZE)
#define TOTAL_IMAGE_HEADER_SIZE		(MAX_PARTITION_NUMBER * IMAGE_HEADER_SIZE)
#define TOTAL_HEADER_SIZE			(IMAGE_HEADER_TABLE_SIZE + \
									 TOTAL_IMAGE_HEADER_SIZE + \
									 TOTAL_PARTITION_HEADER_SIZE + 64)

/* Boot ROM Image defines */
#define IMAGE_WIDTH_CHECK_OFFSET        (0x020)	/**< 0xaa995566 Width Detection word */
#define IMAGE_IDENT_OFFSET              (0x024) /**< 0x584C4E58 "XLNX" */
#define IMAGE_ENC_FLAG_OFFSET           (0x028) /**< 0xA5C3C5A3 */
#define IMAGE_USR_DEF_OFFSET            (0x02C)	/**< undefined  could be used as  */
#define IMAGE_SOURCE_ADDR_OFFSET        (0x030)	/**< start address of image  */
#define IMAGE_BYTE_LEN_OFFSET           (0x034)	/**< length of image> in bytes  */
#define IMAGE_DEST_ADDR_OFFSET          (0x038)	/**< destination address in OCM */
#define IMAGE_EXECUTE_ADDR_OFFSET       (0x03c)	/**< address to start executing at */
#define IMAGE_TOT_BYTE_LEN_OFFSET       (0x040)	/**< total length of image in bytes */
#define IMAGE_QSPI_CFG_WORD_OFFSET      (0x044)	/**< QSPI configuration data */
#define IMAGE_CHECKSUM_OFFSET           (0x048) /**< Header Checksum offset */
#define IMAGE_IDENT                     (0x584C4E58) /**< XLNX pattern */

/* Partition Header defines */
#define PARTITION_IMAGE_WORD_LEN_OFFSET	0x00	/* Word length of image */
#define PARTITION_DATA_WORD_LEN_OFFSET	0x04	/* Word length of data */
#define PARTITION_WORD_LEN_OFFSET		0x08	/* Word length of partition */
#define PARTITION_LOAD_ADDRESS_OFFSET	0x0C	/* Load addr in DDR	*/
#define PARTITION_EXEC_ADDRESS_OFFSET	0x10	/* Addr to start executing */
#define PARTITION_ADDR_OFFSET			0x14	/* Partition word offset */
#define PARTITION_ATTRIBUTE_OFFSET		0x18	/* Partition type */
#define PARTITION_HDR_CHECKSUM_OFFSET	0x3C	/* Header Checksum offset */
#define PARTITION_HDR_CHECKSUM_WORD_COUNT 0xF	/* Checksum word count */
#define PARTITION_HDR_WORD_COUNT		0x10	/* Header word len */
#define PARTITION_HDR_TOTAL_LEN			0x40	/* One partition hdr length*/

/* Attribute word defines */
#define ATTRIBUTE_IMAGE_TYPE_MASK		0xF0	/* Destination Device type */
#define ATTRIBUTE_PS_IMAGE_MASK			0x10	/* Code partition */
#define ATTRIBUTE_PL_IMAGE_MASK			0x20	/* Bit stream partition */
#define ATTRIBUTE_CHECKSUM_TYPE_MASK	0x7000	/* Checksum Type */
#define ATTRIBUTE_RSA_PRESENT_MASK		0x8000	/* RSA Signature Present */
#define ATTRIBUTE_PARTITION_OWNER_MASK	0x30000	/* Partition Owner */

#define ATTRIBUTE_PARTITION_OWNER_FSBL	0x00000	/* FSBL Partition Owner */



#define MAXIMUM_IMAGE_WORD_LEN			0x40000000

#define PARTITION_HEADER_LENGTH			(16)

CPartitionHeaderInfo::CPartitionHeaderInfo()
{
	m_pByteArray = NULL;
	m_bHaveDataStream = false;
	m_bHaveAnalyseStream = false;
	m_u32FsblLength = 0;
	m_u32PartitionHeaderStartAddr = 0;
	m_u32PartitionCount = 0;
}

CPartitionHeaderInfo::CPartitionHeaderInfo(QByteArray* pByteArray):m_pByteArray(pByteArray)
{
	m_bHaveDataStream = false;
	m_bHaveAnalyseStream = false;
	if (pByteArray != NULL)
		m_bHaveDataStream = true;
	else
		return;
	
	m_u32PartitionHeaderStartAddr = 0;
	m_u32FsblLength = 0;
	m_u32PartitionCount = 0;

	AnalyseHeaderInfo();

	m_bHaveAnalyseStream = true;
}

void CPartitionHeaderInfo::AnaylseByteArray(QByteArray* pByteArray)
{
	m_bHaveDataStream = false;
	if (pByteArray != NULL)
		m_bHaveDataStream = true;
	else
		return;

	m_pByteArray = pByteArray;

	m_u32PartitionHeaderStartAddr = 0;
	m_u32FsblLength = 0;
	m_u32PartitionCount = 0;

	if (m_bHaveAnalyseStream == true)
	{
		m_vPartHeader.clear();
	}

	AnalyseHeaderInfo();

	m_bHaveAnalyseStream = true;
}

unsigned int CPartitionHeaderInfo::GetValue(unsigned int u32DataOffset)
{
	unsigned int temp = 0;

	temp =(*m_pByteArray)[u32DataOffset] & 0x000000FF;
	temp += (((*m_pByteArray)[u32DataOffset + 1] << 8) & 0x0000FF00);
	temp += (((*m_pByteArray)[u32DataOffset + 2] << 16) & 0x00FF0000);
	temp += (((*m_pByteArray)[u32DataOffset + 3] << 24) & 0xFF000000);

	return temp;
}

unsigned int CPartitionHeaderInfo::GetPartitionHeader(unsigned int u32DataOffset)
{
	unsigned int u32TmpArray[PARTITION_HEADER_LENGTH] = { 0 };

	for (int i = 0; i < PARTITION_HEADER_LENGTH; i++)
	{
		u32TmpArray[i] = GetValue(u32DataOffset + i*4);
	}

	if (u32TmpArray[PARTITION_HDR_CHECKSUM_WORD_COUNT] != 0xFFFFFFFF)
	{
		m_u32PartitionCount++;
		
		STRU_PART_HEADER stHeader;
		memcpy(&stHeader, u32TmpArray, sizeof(STRU_PART_HEADER));

		m_vPartHeader.append(stHeader);

		return -1;
	}

	for (int j = 0; j < PARTITION_HDR_WORD_COUNT - 1; j++)
	{
		if (u32TmpArray[j] != 0x00)
		{
			m_u32PartitionCount++;

			STRU_PART_HEADER stHeader;
			memcpy(&stHeader, u32TmpArray, sizeof(STRU_PART_HEADER));

			m_vPartHeader.append(stHeader);

			return -1;
		}
	}

	return 0;
}

void CPartitionHeaderInfo::AnalyseAttribute()
{
	for (int i = 0; i < m_vPartHeader.count(); i++)
	{
		unsigned int PartitionAttr = m_vPartHeader[i].u32PartitionAttr;
		QString strAttrInfo;
		
		/*
		* if FSBL is not the owner of partition,
		* skip this partition, continue with next partition
		*/

		if ((PartitionAttr & ATTRIBUTE_PARTITION_OWNER_MASK) != ATTRIBUTE_PARTITION_OWNER_FSBL)
		{
			strAttrInfo += "Skipping this partition \r\n";
		}

		if (PartitionAttr & ATTRIBUTE_PL_IMAGE_MASK)
		{
			strAttrInfo += "This is a Bitstream \r\n";
		}

		if (PartitionAttr & ATTRIBUTE_PS_IMAGE_MASK)
		{
			strAttrInfo += "This is a Application \r\n";
		}

		/*
		* Encrypted partition will have different value
		* for Image length and data length
		*/

		if (m_vPartHeader[i].u32DataWordLen != m_vPartHeader[i].u32ImageWordLen)
		{
			strAttrInfo += "Partition is Encrypted \r\n";
		}

		/*
		* Check for partition checksum check
		*/
		if (PartitionAttr & ATTRIBUTE_CHECKSUM_TYPE_MASK)
		{
			strAttrInfo += "Partition need Check Sum \r\n";
		}

		/*
		* RSA signature check
		*/
		if (PartitionAttr & ATTRIBUTE_RSA_PRESENT_MASK)
		{
			strAttrInfo += "Partition RSA Signed \r\n";
		}

		m_vPartAttriInfo.append(strAttrInfo);
	}
}

void CPartitionHeaderInfo::AnalyseHeaderInfo()
{
	if (m_bHaveDataStream != true)
		return;

	if (m_pByteArray->size() < IMAGE_TOT_BYTE_LEN_OFFSET + 4)
		return;

	m_u32FsblLength = GetValue(IMAGE_TOT_BYTE_LEN_OFFSET);

	m_u32PartitionHeaderStartAddr = GetValue(IMAGE_PHDR_OFFSET);

	unsigned int tmp = -1;
	unsigned int i = 0;
	do
	{
		tmp = GetPartitionHeader(m_u32PartitionHeaderStartAddr + i*16*4);
		i++;
	} while (tmp == -1);
	
	AnalyseAttribute();
}

int CPartitionHeaderInfo::ValidPartitionHeader(STRU_PART_HEADER& stHeader)
{
	unsigned int* pTmp = (unsigned int*)&stHeader;

	for (int i = 0; i < PARTITION_HDR_WORD_COUNT; i++)
	{	
		if (pTmp[i] != 0)
			return -1;
	}

	unsigned int chk = 0;
	for (int j = 0; j < PARTITION_HDR_CHECKSUM_WORD_COUNT; j++)
	{
		chk += pTmp[j];
	}
	chk ^= 0xFFFFFFFF;
	if (pTmp[PARTITION_HDR_CHECKSUM_WORD_COUNT] != chk)
		return -2;

	if (stHeader.u32ImageWordLen > MAXIMUM_IMAGE_WORD_LEN)
		return -3;

	return 0;
}

QString CPartitionHeaderInfo::GetAttriInfo(unsigned int index)
{
	return m_vPartAttriInfo[index];
}

unsigned int CPartitionHeaderInfo::GetFsblLength()
{
	return m_u32FsblLength;
}

unsigned int CPartitionHeaderInfo::GetPartitionHeaderStartAddr()
{
	return m_u32PartitionHeaderStartAddr;
}

unsigned int CPartitionHeaderInfo::GetPartitionCount()
{
	return m_u32PartitionCount;
}

STRU_PART_HEADER CPartitionHeaderInfo::GetPartition(unsigned int index)
{
	return m_vPartHeader[index];
}

bool CPartitionHeaderInfo::isAnalysed()
{
	return m_bHaveAnalyseStream;
}