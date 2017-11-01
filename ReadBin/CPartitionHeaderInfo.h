#pragma once

#include <QByteArray>
#include <QVector>

typedef struct
{
	unsigned int u32ImageWordLen;		/* 0x0 */
	unsigned int u32DataWordLen;		/* 0x4 */
	unsigned int u32PartitionWordLen;	/* 0x8 */
	unsigned int u32LoadAddr;			/* 0xC */
	unsigned int u32ExecAddr;			/* 0x10 */
	unsigned int u32PartitionStart;		/* 0x14 */
	unsigned int u32PartitionAttr;		/* 0x18 */
	unsigned int u32SectionCount;		/* 0x1C */
	unsigned int u32CheckSumOffset;		/* 0x20 */
	unsigned int u32Pads1[1];
	unsigned int u32ACOffset;			/* 0x28 */
	unsigned int u32Pads2[4];
	unsigned int u32CheckSum;			/* 0x3C */
}STRU_PART_HEADER;

class CPartitionHeaderInfo
{
public:
	CPartitionHeaderInfo();
	CPartitionHeaderInfo(QByteArray* pByteArray);

	void AnaylseByteArray(QByteArray* pByteArray);

	bool isAnalysed();

	unsigned int GetFsblLength();
	unsigned int GetPartitionHeaderStartAddr();
	unsigned int GetPartitionCount();
	

	STRU_PART_HEADER GetPartition(unsigned int index);
	QString GetAttriInfo(unsigned int index);

private:
	bool m_bHaveAnalyseStream;

	void AnalyseHeaderInfo();
	void AnalyseAttribute();
	QByteArray* m_pByteArray;
	bool m_bHaveDataStream;
	unsigned int GetValue(unsigned int u32DataOffset);
	unsigned int GetPartitionHeader(unsigned int u32DataOffset);
	int ValidPartitionHeader(STRU_PART_HEADER& stHeader);

	unsigned int m_u32FsblLength;
	unsigned int m_u32PartitionHeaderStartAddr;
	unsigned int m_u32PartitionCount;

	QVector<STRU_PART_HEADER> m_vPartHeader;
	QVector<QString> m_vPartAttriInfo;
};