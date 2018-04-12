// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017 The Helios Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef helios_QT_heliosADDRESSVALIDATOR_H
#define helios_QT_heliosADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class HeliosAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HeliosAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Helios address widget validator, checks for a valid helios address.
 */
class HeliosAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HeliosAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // helios_QT_heliosADDRESSVALIDATOR_H
