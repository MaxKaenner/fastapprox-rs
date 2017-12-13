/*=====================================================================*
 *                   Copyright (C) 2012 Paul Mineiro                   *
 * All rights reserved.                                                *
 *                                                                     *
 * Redistribution and use in source and binary forms, with             *
 * or without modification, are permitted provided that the            *
 * following conditions are met:                                       *
 *                                                                     *
 *     * Redistributions of source code must retain the                *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer.                                       *
 *                                                                     *
 *     * Redistributions in binary form must reproduce the             *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer in the documentation and/or            *
 *     other materials provided with the distribution.                 *
 *                                                                     *
 *     * Neither the name of Paul Mineiro nor the names                *
 *     of other contributors may be used to endorse or promote         *
 *     products derived from this software without specific            *
 *     prior written permission.                                       *
 *                                                                     *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND              *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,         *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES               *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE             *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER               *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,                 *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES            *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE           *
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR                *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF          *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY              *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             *
 * POSSIBILITY OF SUCH DAMAGE.                                         *
 *                                                                     *
 * Contact: Paul Mineiro <paul@mineiro.com>                            *
 *=====================================================================*/

 #include <string.h>
 #include <stdint.h>
 #include <math.h>

// Underflow of exponential is common practice in numerical routines,
// so handle it here.

float
fastpow2 (float p)
{
  float offset = (p < 0) ? 1.0f : 0.0f;
  float clipp = (p < -126) ? -126.0f : p;
  int w = clipp;
  float z = clipp - w + offset;
  union { uint32_t i; float f; } v = { (uint32_t) ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };

  return v.f;
}

float
fastexp (float p)
{
  return fastpow2 (1.442695040f * p);
}

float
fasterpow2 (float p)
{
  float clipp = (p < -126) ? -126.0f : p;
  union { uint32_t i; float f; } v = { (uint32_t) ( (1 << 23) * (clipp + 126.94269504f) ) };
  return v.f;
}

float
fasterexp (float p)
{
  return fasterpow2 (1.442695040f * p);
}

float 
fastlog2 (float x)
{
  union { float f; uint32_t i; } vx = { x };
  union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;

  return y - 124.22551499f
           - 1.498030302f * mx.f 
           - 1.72587999f / (0.3520887068f + mx.f);
}

float
fastlog (float x)
{
  return 0.69314718f * fastlog2 (x);
}

float 
fasterlog2 (float x)
{
  union { float f; uint32_t i; } vx = { x };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;
  return y - 126.94269504f;
}

float
fasterlog (float x)
{
  union { float f; uint32_t i; } vx = { x };
  float y = vx.i;
  y *= 8.2629582881927490e-8f;
  return y - 87.989971088f;
}

float
fastpow (float x,
         float p)
{
  return fastpow2 (p * fastlog2 (x));
}

float
fasterpow (float x,
           float p)
{
  return fasterpow2 (p * fasterlog2 (x));
}

float
fastsigmoid (float x)
{
  return 1.0f / (1.0f + fastexp (-x));
}

float
fastersigmoid (float x)
{
  return 1.0f / (1.0f + fasterexp (-x));
}

float
fastlgamma (float x)
{
  float logterm = fastlog (x * (1.0f + x) * (2.0f + x));
  float xp3 = 3.0f + x;

  return - 2.081061466f 
         - x 
         + 0.0833333f / xp3 
         - logterm 
         + (2.5f + x) * fastlog (xp3);
}

float
fasterlgamma (float x)
{
  return - 0.0810614667f 
         - x
         - fasterlog (x)
         + (0.5f + x) * fasterlog (1.0f + x);
}

float
fastdigamma (float x)
{
  float twopx = 2.0f + x;
  float logterm = fastlog (twopx);

  return (-48.0f + x * (-157.0f + x * (-127.0f - 30.0f * x))) /
         (12.0f * x * (1.0f + x) * twopx * twopx)
         + logterm;
}

float
fasterdigamma (float x)
{
  float onepx = 1.0f + x;

  return -1.0f / x - 1.0f / (2 * onepx) + fasterlog (onepx);
}

float
fasterfc (float x)
{
  static const float k = 3.3509633149424609f;
  static const float a = 0.07219054755431126f;
  static const float b = 15.418191568719577f;
  static const float c = 5.609846028328545f;

  union { float f; uint32_t i; } vc = { c * x };
  float xsq = x * x;
  float xquad = xsq * xsq;

  vc.i |= 0x80000000;

  return 2.0f / (1.0f + fastpow2 (k * x)) - a * x * (b * xquad - 1.0f) * fasterpow2 (vc.f);
}

float
fastererfc (float x)
{
  static const float k = 3.3509633149424609f;

  return 2.0f / (1.0f + fasterpow2 (k * x));
}

float
fasterf (float x)
{
  return 1.0f - fasterfc (x);
}

float
fastererf (float x)
{
  return 1.0f - fastererfc (x);
}

float
fastinverseerf (float x)
{
  static const float invk = 0.30004578719350504f;
  static const float a = 0.020287853348211326f;
  static const float b = 0.07236892874789555f;
  static const float c = 0.9913030456864257f;
  static const float d = 0.8059775923760193f;

  float xsq = x * x;

  return invk * fastlog2 ((1.0f + x) / (1.0f - x)) 
       + x * (a - b * xsq) / (c - d * xsq);
}

float
fasterinverseerf (float x)
{
  static const float invk = 0.30004578719350504f;

  return invk * fasterlog2 ((1.0f + x) / (1.0f - x));
}

float
fastsinh (float p)
{
  return 0.5f * (fastexp (p) - fastexp (-p));
}

float
fastersinh (float p)
{
  return 0.5f * (fasterexp (p) - fasterexp (-p));
}

float
fastcosh (float p)
{
  return 0.5f * (fastexp (p) + fastexp (-p));
}

float
fastercosh (float p)
{
  return 0.5f * (fasterexp (p) + fasterexp (-p));
}

float
fasttanh (float p)
{
  return -1.0f + 2.0f / (1.0f + fastexp (-2.0f * p));
}

float
fastertanh (float p)
{
  return -1.0f + 2.0f / (1.0f + fasterexp (-2.0f * p));
}

// these functions compute the upper branch aka W_0

float
fastlambertw (float x)
{
  static const float threshold = 2.26445f;

  float c = (x < threshold) ? 1.546865557f : 1.0f;
  float d = (x < threshold) ? 2.250366841f : 0.0f;
  float a = (x < threshold) ? -0.737769969f : 0.0f;

  float logterm = fastlog (c * x + d);
  float loglogterm = fastlog (logterm);

  float minusw = -a - logterm + loglogterm - loglogterm / logterm;
  float expminusw = fastexp (minusw);
  float xexpminusw = x * expminusw;
  float pexpminusw = xexpminusw - minusw;

  return (2.0f * xexpminusw - minusw * (4.0f * xexpminusw - minusw * pexpminusw)) /
         (2.0f + pexpminusw * (2.0f - minusw));
}

float
fasterlambertw (float x)
{
  static const float threshold = 2.26445f;

  float c = (x < threshold) ? 1.546865557f : 1.0f;
  float d = (x < threshold) ? 2.250366841f : 0.0f;
  float a = (x < threshold) ? -0.737769969f : 0.0f;

  float logterm = fasterlog (c * x + d);
  float loglogterm = fasterlog (logterm);

  float w = a + logterm - loglogterm + loglogterm / logterm;
  float expw = fasterexp (-w);

  return (w * w + expw * x) / (1.0f + w);
}

float
fastlambertwexpx (float x)
{
  static const float k = 1.1765631309f;
  static const float a = 0.94537622168f;

  float logarg = fmaxf (x, k);
  float powarg = (x < k) ? a * (x - k) : 0;

  float logterm = fastlog (logarg);
  float powterm = fasterpow2 (powarg);  // don't need accuracy here

  float w = powterm * (logarg - logterm + logterm / logarg);
  float logw = fastlog (w);
  float p = x - logw;

  return w * (2.0f + p + w * (3.0f + 2.0f * p)) /
         (2.0f - p + w * (5.0f + 2.0f * w));
}

float
fasterlambertwexpx (float x)
{
  static const float k = 1.1765631309f;
  static const float a = 0.94537622168f;

  float logarg = fmaxf (x, k);
  float powarg = (x < k) ? a * (x - k) : 0;

  float logterm = fasterlog (logarg);
  float powterm = fasterpow2 (powarg);

  float w = powterm * (logarg - logterm + logterm / logarg);
  float logw = fasterlog (w);

  return w * (1.0f + x - logw) / (1.0f + w);
}

// http://www.devmaster.net/forums/showthread.php?t=5784
// fast sine variants are for x \in [ -\pi, pi ]
// fast cosine variants are for x \in [ -\pi, pi ]
// fast tangent variants are for x \in [ -\pi / 2, pi / 2 ]
// "full" versions of functions handle the entire range of inputs
// although the range reduction technique used here will be hopelessly
// inaccurate for |x| >> 1000
//
// WARNING: fastsinfull, fastcosfull, and fasttanfull can be slower than
// libc calls on older machines (!) and on newer machines are only 
// slighly faster.  however:
//   * vectorized versions are competitive
//   * faster full versions are competitive

float
fastsin (float x)
{
  static const float fouroverpi = 1.2732395447351627f;
  static const float fouroverpisq = 0.40528473456935109f;
  static const float q = 0.78444488374548933f;
  union { float f; uint32_t i; } p = { 0.20363937680730309f };
  union { float f; uint32_t i; } r = { 0.015124940802184233f };
  union { float f; uint32_t i; } s = { -0.0032225901625579573f };

  union { float f; uint32_t i; } vx = { x };
  uint32_t sign = vx.i & 0x80000000;
  vx.i = vx.i & 0x7FFFFFFF;

  float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;
  float qpproxsq = qpprox * qpprox;

  p.i |= sign;
  r.i |= sign;
  s.i ^= sign;

  return q * qpprox + qpproxsq * (p.f + qpproxsq * (r.f + qpproxsq * s.f));
}

float
fastersin (float x)
{
  static const float fouroverpi = 1.2732395447351627f;
  static const float fouroverpisq = 0.40528473456935109f;
  static const float q = 0.77633023248007499f;
  union { float f; uint32_t i; } p = { 0.22308510060189463f };

  union { float f; uint32_t i; } vx = { x };
  uint32_t sign = vx.i & 0x80000000;
  vx.i &= 0x7FFFFFFF;

  float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;

  p.i |= sign;

  return qpprox * (q + p.f * qpprox);
}

float
fastsinfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  return fastsin ((half + k) * twopi - x);
}

float
fastersinfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  return fastersin ((half + k) * twopi - x);
}

float
fastcos (float x)
{
  static const float halfpi = 1.5707963267948966f;
  static const float halfpiminustwopi = -4.7123889803846899f;
  float offset = (x > halfpi) ? halfpiminustwopi : halfpi;
  return fastsin (x + offset);
}

float
fastercos (float x)
{
  static const float twooverpi = 0.63661977236758134f;
  static const float p = 0.54641335845679634f;

  union { float f; uint32_t i; } vx = { x };
  vx.i &= 0x7FFFFFFF;

  float qpprox = 1.0f - twooverpi * vx.f;

  return qpprox + p * qpprox * (1.0f - qpprox * qpprox);
}

float
fastcosfull (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastsinfull (x + halfpi);
}

float
fastercosfull (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastersinfull (x + halfpi);
}

float
fasttan (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastsin (x) / fastsin (x + halfpi);
}

float
fastertan (float x)
{
  return fastersin (x) / fastercos (x);
}

float
fasttanfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  float xnew = x - (half + k) * twopi;

  return fastsin (xnew) / fastcos (xnew);
}

float
fastertanfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  float xnew = x - (half + k) * twopi;

  return fastersin (xnew) / fastercos (xnew);
}
