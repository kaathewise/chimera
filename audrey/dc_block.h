/*
Copyright (c) 2020 Electrosmith, Corp

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#ifndef AUDREY_DC_BLOCK_H_
#define AUDREY_DC_BLOCK_H_

namespace audrey {
/** Removes DC component of a signal
 */
class DCBlock {
 public:
  DCBlock() {}
  ~DCBlock() {}

  /** Initializes DCBlock module
   */
  void Init(float sample_rate);

  /** performs DCBlock Process
   */
  float Process(float in);

 private:
  float input_, output_, gain_;
};
}  // namespace audrey

#endif  // AUDREY_DC_BLOCK_H_
