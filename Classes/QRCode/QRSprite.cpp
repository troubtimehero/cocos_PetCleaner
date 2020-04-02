//
//  QRSprite.cpp
//  CGame
//
//  Created by neo on 15/12/16.
//
//

#include "QRSprite.h"
#include "QR_Encode.h"




static DrawNode* QRSpritecreate(const char *code)
	{
	if (code == NULL)
		return NULL;
	CQR_Encode m_QREncode;
	bool bRet = m_QREncode.EncodeData(0, 0, 1, -1, code);

	// 添加生成图像代码, 这边采用的是CCDrawNode这个类直接绘制
	if (bRet)
		{

		int nSize = 5;
		int originalSize = m_QREncode.m_nSymbleSize + (QR_MARGIN * 2);
		DrawNode *pQRNode = DrawNode::create();


#if COCOS2D_VERSION <0x00030000

		CCPoint pt[6];
		ccColor4F color;

		pt[0] = ccp(0, 0);
		pt[1] = ccp((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize);
		pt[2] = ccp((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, 0);

		pt[3] = pt[0];
		pt[4] = ccp(0, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize);
		pt[5] = pt[1];
		color = ccc4f(1, 1, 1, 1);
		pQRNode->drawPolygon(pt, 6, color, 0, color);

		for (int i = 0; i < m_QREncode.m_nSymbleSize; ++i)
			{
			for (int j = 0; j < m_QREncode.m_nSymbleSize; ++j)
				{
				pt[0] = ccp((i + QR_MARGIN)*nSize, (j + QR_MARGIN)*nSize);
				pt[1] = ccp(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize);
				pt[2] = ccp(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 0)*nSize);

				pt[3] = pt[0];
				pt[4] = ccp(((i + QR_MARGIN) + 0)*nSize, ((j + QR_MARGIN) + 1)*nSize);
				pt[5] = pt[1];
				if (m_QREncode.m_byModuleData[i][j] == 1)
					{
					color = ccc4f(0, 0, 0, 1);
					}
				else
					{
					color = ccc4f(1, 1, 1, 1);
					}
				pQRNode->drawPolygon(pt, 6, color, 0, color);
				}
			}
#else
		// 绘制像素点
		for (int i = 0; i < m_QREncode.m_nSymbleSize; ++i)
			{
			for (int j = 0; j < m_QREncode.m_nSymbleSize; ++j)
				{
				if (m_QREncode.m_byModuleData[i][j] == 1)
					{
					pQRNode->drawSolidRect(Vec2((i + QR_MARGIN)*nSize, (j + QR_MARGIN)*nSize), Vec2(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize), Color4F(0, 0, 0, 1));
					}
				else
					{
					pQRNode->drawSolidRect(Vec2((i + QR_MARGIN)*nSize, (j + QR_MARGIN)*nSize), Vec2(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize), Color4F(1, 1, 1, 1));
					}
				}
			}
		// 绘制外框
		pQRNode->drawSolidRect(Vec2(0, 0), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (QR_MARGIN)*nSize), Color4F(1, 1, 1, 1));
		pQRNode->drawSolidRect(Vec2(0, 0), Vec2((QR_MARGIN)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize), Color4F(1, 1, 1, 1));
		pQRNode->drawSolidRect(Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN)*nSize, 0), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize), Color4F(1, 1, 1, 1));
		pQRNode->drawSolidRect(Vec2(0, (m_QREncode.m_nSymbleSize + QR_MARGIN)*nSize), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize), Color4F(1, 1, 1, 1));

		pQRNode->setContentSize(Size((m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2)*nSize));
#endif
		return pQRNode;
		}
	return NULL;
}

DrawNode* QRSprite::create(const char *code)
{
	return QRSpritecreate(code);
}
