/* MIT License
 *
 * Copyright (c) 2018 Assign Onward
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef KVKEYS_H
#define KVKEYS_H

#define AOK_PROTOCOL         1 // Basic level of protocol, identifies functionality
#define AOK_PROTOCOL_REV     2 // Revision of the protocol, may indicate additional types supported
#define AOK_TEXT_SYMBOL      3 // Short unique symbol that uniquely identifies the chain e.g. TⒶ1a
#define AOK_DESCRIPTION      4 // UTF-8 Text description of the chain
#define AOK_ICON             5 // Image suitable for icon use to represent the chain
#define AOK_IMAGE            6 // Large format image to represent the chain
#define AOK_STARTING_SHARES  7 // Starting number of shares
#define AOK_MIN_BLOCK_INT    8 // Minimuim block interval time
#define AOK_N_COINS_TOTAL    9 // Number of coins that the sum of all shares outstanding represents
#define AOK_RECORDING_TAX   10 // Recording Tax in coins per byte (usually a very small number)

#endif // KVKEYS_H
