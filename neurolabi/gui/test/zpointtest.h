#ifndef ZPOINTTEST_H
#define ZPOINTTEST_H

#include "ztestheader.h"
#include "zintpoint.h"

#ifdef _USE_GTEST_

TEST(ZIntPoint, Basic)
{
  ZIntPoint pt;
  ASSERT_TRUE(pt.isZero());

  pt.invalidate();
  ASSERT_FALSE(pt.isValid());
}

TEST(ZIntPoint, Operator)
{
  ZIntPoint pt1(0, 1, 2);
  pt1 = pt1 + 1;
  ASSERT_EQ(pt1, ZIntPoint(1, 2, 3));

  pt1.invalidate();
  ASSERT_FALSE((-pt1).isValid());

  ASSERT_FALSE((pt1 + 1).isValid());

  pt1 += ZIntPoint(0, 1, 2);
  ASSERT_FALSE(pt1.isValid());

  pt1.set(0, 1, 2);
  ASSERT_TRUE(pt1.isValid());

  ASSERT_FALSE((pt1 + INT_MIN).isValid());

  pt1 += ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(10, 21, 32));


  pt1 = ZIntPoint(1, 2, 3) + ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(11, 22, 33));


  pt1.set(0, 1, 2);
  pt1 = pt1 - 1;
  ASSERT_EQ(pt1, ZIntPoint(-1, 0, 1));
  ASSERT_EQ(-pt1, ZIntPoint(1, 0, -1));

  pt1.invalidate();
  ASSERT_FALSE((pt1 - 1).isValid());

  pt1 -= ZIntPoint(0, 1, 2);
  ASSERT_FALSE(pt1.isValid());

  pt1.set(0, 1, 2);
  ASSERT_TRUE(pt1.isValid());

  ASSERT_FALSE((pt1 - INT_MIN).isValid());

  pt1 -= ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(-10, -19, -28));

  pt1 = ZIntPoint(1, 2, 3) - ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(-9, -18, -27));

  pt1.set(0, 1, 2);
  pt1 = pt1 * 2;
  ASSERT_EQ(pt1, ZIntPoint(0, 2, 4));

  pt1.invalidate();
  ASSERT_FALSE((pt1 * 2).isValid());

  pt1 *= ZIntPoint(1, 2, 3);
  ASSERT_FALSE(pt1.isValid());

  pt1.set(0, 1, 2);
  ASSERT_TRUE(pt1.isValid());

  ASSERT_FALSE((pt1 * INT_MIN).isValid());

  pt1 *= ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(0, 20, 60));


  pt1 = ZIntPoint(1, 2, 3) * ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(10, 40, 90));


  pt1.set(0, 1, 2);
  pt1 = pt1 / 2;
  ASSERT_EQ(pt1, ZIntPoint(0, 0, 1));

  pt1.invalidate();
  ASSERT_FALSE((pt1 / 2).isValid());

  pt1 /= ZIntPoint(1, 2, 3);
  ASSERT_FALSE(pt1.isValid());

  pt1.set(0, 1, 2);
  ASSERT_TRUE(pt1.isValid());

  ASSERT_FALSE((pt1 / INT_MIN).isValid());

  pt1 /= ZIntPoint(10, 20, 30);
  ASSERT_EQ(pt1, ZIntPoint(0, 0, 0));


  pt1 =  ZIntPoint(10, 20, 30) / ZIntPoint(1, 2, 3);
  ASSERT_EQ(pt1, ZIntPoint(10, 10, 10));
}

#endif


#endif // ZPOINTTEST_H