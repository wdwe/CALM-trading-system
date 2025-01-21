add_library(tws STATIC
        ContractCondition.cpp
        DefaultEWrapper.cpp
        EClient.cpp
        EClientSocket.cpp
        EDecoder.cpp
        EMessage.cpp
        EMutex.cpp
        EOrderDecoder.cpp
        EReader.cpp
        EReaderOSSignal.cpp
        ESocket.cpp
        MarginCondition.cpp
        OperatorCondition.cpp
        OrderCondition.cpp
        PercentChangeCondition.cpp
        PriceCondition.cpp
        SoftDollarTier.cpp
        StdAfx.cpp
        TimeCondition.cpp
        VolumeCondition.cpp
        executioncondition.cpp
)

target_link_directories(tws PUBLIC lib)
#set(THREADS_PREFER_PTHREAD_FLAG ON) # for pthread
find_package(Threads) # for pthread, but it seems to compile fine w/o these two lines
target_link_libraries(tws
        PUBLIC bid
        PUBLIC Threads::Threads
)
