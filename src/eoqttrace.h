#ifndef EOQTTRACE_H
#define EOQTTRACE_H

/**
    @file eoqttrace.h
    @brief Debug printout macros for Qt.

    @details Enable by adding the following lines to .PRO.
    @code
    DEFINES += ENABLE_FUNC_TRACE ENABLE_TRACE
    @endcode

    @def FUTR()
    @brief Place at the beginning of the function.

    @def FUTR_BEGIN()
    @brief Place at the beginning of the function.

    @def FUTR_END()
    @brief Place at the end of the function.

    @def FUNC_TRACE(param)
    @brief Place at the beginning of the function.

    @def TRACE(param)
    @brief Simple string trace.

    @def TRACEn(param1, param2, paramN)
    @brief Formatting string + parameter(s).

*/
#include <QString>
#include <QDebug>

#ifdef __GNUC__
    /* Supported by Gnu C */
    #define EO_FUNCTION __PRETTY_FUNCTION__
#else
    /* Supported by GNU C and VC++ */
    #define EO_FUNCTION __FUNCTION__
#endif

#ifdef ENABLE_FUNC_TRACE
    #define FUNC_TRACE(param) qDebug() << "[" + QString("%1").arg(param) + "]"
    #define FUTR() qDebug() << "[" << EO_FUNCTION << "]";
    #define FUTR_BEGIN() qDebug() << "[" << EO_FUNCTION << "]" << "BEGIN";
    #define FUTR_END() qDebug() << "[" << EO_FUNCTION << "]" << "END";
#else
    #define FUNC_TRACE(param)
    #define FUTR()
    #define FUTR_BEGIN()
    #define FUTR_END()
#endif

#ifdef ENABLE_TRACE
    // stream object supporting streaming to qDebug
    #define STRACE(param) qDebug() << param

    #define TRACE(param) qDebug() << " " << QString("%1").arg(param)
    #define TRACE1(format, param) qDebug() << " " << QString(format).arg(param)
    #define TRACE2(format, param1, param2) qDebug() << " " \
        << QString(format).arg(param1).arg(param2)
    #define TRACE3(format, param1, param2, param3) qDebug() << " " \
        << QString(format).arg(param1).arg(param2).arg(param3)
    #define TRACE4(format, param1, param2, param3, param4) qDebug() << " " \
        << QString(format).arg(param1).arg(param2).arg(param3).arg(param4)
#else
    #define STRACE(param)
    #define TRACE(param)
    #define TRACE1(format, param)
    #define TRACE2(format, param1, param2)
    #define TRACE3(format, param1, param2, param3)
    #define TRACE4(format, param1, param2, param3, param4)
#endif

#endif // EOQTTRACE_H
