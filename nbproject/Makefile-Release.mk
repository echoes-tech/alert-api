#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/AddonResource.o \
	${OBJECTDIR}/src/AlertResource.o \
	${OBJECTDIR}/src/AssetResource.o \
	${OBJECTDIR}/src/Conf.o \
	${OBJECTDIR}/src/CriterionResource.o \
	${OBJECTDIR}/src/FilterResource.o \
	${OBJECTDIR}/src/GroupResource.o \
	${OBJECTDIR}/src/InformationDataResource.o \
	${OBJECTDIR}/src/InformationResource.o \
	${OBJECTDIR}/src/JsonSerializer.o \
	${OBJECTDIR}/src/MediaResource.o \
	${OBJECTDIR}/src/MessageResource.o \
	${OBJECTDIR}/src/OptionResource.o \
	${OBJECTDIR}/src/PluginResource.o \
	${OBJECTDIR}/src/ProbeResource.o \
	${OBJECTDIR}/src/PublicApiResource.o \
	${OBJECTDIR}/src/RoleResource.o \
	${OBJECTDIR}/src/SearchResource.o \
	${OBJECTDIR}/src/SourceResource.o \
	${OBJECTDIR}/src/UnitResource.o \
	${OBJECTDIR}/src/UserResource.o \
	${OBJECTDIR}/src/itooki/ItookiAckReceiver.o \
	${OBJECTDIR}/src/itooki/ItookiAswReceiver.o \
	${OBJECTDIR}/src/itooki/ItookiSMSSender.o \
	${OBJECTDIR}/src/itooki/ItookiSendedReiceiver.o \
	${OBJECTDIR}/src/itooki/PublicItookiResource.o \
	${OBJECTDIR}/src/mail/PublicMailResource.o \
	${OBJECTDIR}/src/mail/mailAssign.o \
	${OBJECTDIR}/src/mail/mailForward.o \
	${OBJECTDIR}/src/mail/mailResolve.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f2

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/default/dist/Release_SharedObject/GNU-Linux-x86 -L../dbo/dist/Release_SharedObject/GNU-Linux-x86 -lwt -lwtdbo -lwtdbopostgres -lwthttp -ldbo -lboost_filesystem -lboost_signals -lboost_system

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api ${OBJECTFILES} ${LDLIBSOPTIONS} -s

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/AddonResource.o: src/AddonResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AddonResource.o src/AddonResource.cpp

${OBJECTDIR}/src/AlertResource.o: src/AlertResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AlertResource.o src/AlertResource.cpp

${OBJECTDIR}/src/AssetResource.o: src/AssetResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AssetResource.o src/AssetResource.cpp

${OBJECTDIR}/src/Conf.o: src/Conf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Conf.o src/Conf.cpp

${OBJECTDIR}/src/CriterionResource.o: src/CriterionResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CriterionResource.o src/CriterionResource.cpp

${OBJECTDIR}/src/FilterResource.o: src/FilterResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FilterResource.o src/FilterResource.cpp

${OBJECTDIR}/src/GroupResource.o: src/GroupResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GroupResource.o src/GroupResource.cpp

${OBJECTDIR}/src/InformationDataResource.o: src/InformationDataResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InformationDataResource.o src/InformationDataResource.cpp

${OBJECTDIR}/src/InformationResource.o: src/InformationResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InformationResource.o src/InformationResource.cpp

${OBJECTDIR}/src/JsonSerializer.o: src/JsonSerializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/JsonSerializer.o src/JsonSerializer.cpp

${OBJECTDIR}/src/MediaResource.o: src/MediaResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MediaResource.o src/MediaResource.cpp

${OBJECTDIR}/src/MessageResource.o: src/MessageResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MessageResource.o src/MessageResource.cpp

${OBJECTDIR}/src/OptionResource.o: src/OptionResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/OptionResource.o src/OptionResource.cpp

${OBJECTDIR}/src/PluginResource.o: src/PluginResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PluginResource.o src/PluginResource.cpp

${OBJECTDIR}/src/ProbeResource.o: src/ProbeResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProbeResource.o src/ProbeResource.cpp

${OBJECTDIR}/src/PublicApiResource.o: src/PublicApiResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PublicApiResource.o src/PublicApiResource.cpp

${OBJECTDIR}/src/RoleResource.o: src/RoleResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/RoleResource.o src/RoleResource.cpp

${OBJECTDIR}/src/SearchResource.o: src/SearchResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SearchResource.o src/SearchResource.cpp

${OBJECTDIR}/src/SourceResource.o: src/SourceResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SourceResource.o src/SourceResource.cpp

${OBJECTDIR}/src/UnitResource.o: src/UnitResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UnitResource.o src/UnitResource.cpp

${OBJECTDIR}/src/UserResource.o: src/UserResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UserResource.o src/UserResource.cpp

${OBJECTDIR}/src/itooki/ItookiAckReceiver.o: src/itooki/ItookiAckReceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiAckReceiver.o src/itooki/ItookiAckReceiver.cpp

${OBJECTDIR}/src/itooki/ItookiAswReceiver.o: src/itooki/ItookiAswReceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiAswReceiver.o src/itooki/ItookiAswReceiver.cpp

${OBJECTDIR}/src/itooki/ItookiSMSSender.o: src/itooki/ItookiSMSSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiSMSSender.o src/itooki/ItookiSMSSender.cpp

${OBJECTDIR}/src/itooki/ItookiSendedReiceiver.o: src/itooki/ItookiSendedReiceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiSendedReiceiver.o src/itooki/ItookiSendedReiceiver.cpp

${OBJECTDIR}/src/itooki/PublicItookiResource.o: src/itooki/PublicItookiResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/PublicItookiResource.o src/itooki/PublicItookiResource.cpp

${OBJECTDIR}/src/mail/PublicMailResource.o: src/mail/PublicMailResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/PublicMailResource.o src/mail/PublicMailResource.cpp

${OBJECTDIR}/src/mail/mailAssign.o: src/mail/mailAssign.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/mailAssign.o src/mail/mailAssign.cpp

${OBJECTDIR}/src/mail/mailForward.o: src/mail/mailForward.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/mailForward.o src/mail/mailForward.cpp

${OBJECTDIR}/src/mail/mailResolve.o: src/mail/mailResolve.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/mailResolve.o src/mail/mailResolve.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f2: ${TESTDIR}/tests/AddonResourceTest/AddonResourceTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/tests/AddonResourceTest/AddonResourceTest.o: tests/AddonResourceTest/AddonResourceTest.cpp 
	${MKDIR} -p ${TESTDIR}/tests/AddonResourceTest
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/AddonResourceTest/AddonResourceTest.o tests/AddonResourceTest/AddonResourceTest.cpp


${OBJECTDIR}/main_nomain.o: ${OBJECTDIR}/main.o main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main_nomain.o main.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main.o ${OBJECTDIR}/main_nomain.o;\
	fi

${OBJECTDIR}/src/AddonResource_nomain.o: ${OBJECTDIR}/src/AddonResource.o src/AddonResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/AddonResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AddonResource_nomain.o src/AddonResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/AddonResource.o ${OBJECTDIR}/src/AddonResource_nomain.o;\
	fi

${OBJECTDIR}/src/AlertResource_nomain.o: ${OBJECTDIR}/src/AlertResource.o src/AlertResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/AlertResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AlertResource_nomain.o src/AlertResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/AlertResource.o ${OBJECTDIR}/src/AlertResource_nomain.o;\
	fi

${OBJECTDIR}/src/AssetResource_nomain.o: ${OBJECTDIR}/src/AssetResource.o src/AssetResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/AssetResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AssetResource_nomain.o src/AssetResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/AssetResource.o ${OBJECTDIR}/src/AssetResource_nomain.o;\
	fi

${OBJECTDIR}/src/Conf_nomain.o: ${OBJECTDIR}/src/Conf.o src/Conf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Conf.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Conf_nomain.o src/Conf.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Conf.o ${OBJECTDIR}/src/Conf_nomain.o;\
	fi

${OBJECTDIR}/src/CriterionResource_nomain.o: ${OBJECTDIR}/src/CriterionResource.o src/CriterionResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/CriterionResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CriterionResource_nomain.o src/CriterionResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/CriterionResource.o ${OBJECTDIR}/src/CriterionResource_nomain.o;\
	fi

${OBJECTDIR}/src/FilterResource_nomain.o: ${OBJECTDIR}/src/FilterResource.o src/FilterResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FilterResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FilterResource_nomain.o src/FilterResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FilterResource.o ${OBJECTDIR}/src/FilterResource_nomain.o;\
	fi

${OBJECTDIR}/src/GroupResource_nomain.o: ${OBJECTDIR}/src/GroupResource.o src/GroupResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/GroupResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GroupResource_nomain.o src/GroupResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/GroupResource.o ${OBJECTDIR}/src/GroupResource_nomain.o;\
	fi

${OBJECTDIR}/src/InformationDataResource_nomain.o: ${OBJECTDIR}/src/InformationDataResource.o src/InformationDataResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/InformationDataResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InformationDataResource_nomain.o src/InformationDataResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/InformationDataResource.o ${OBJECTDIR}/src/InformationDataResource_nomain.o;\
	fi

${OBJECTDIR}/src/InformationResource_nomain.o: ${OBJECTDIR}/src/InformationResource.o src/InformationResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/InformationResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InformationResource_nomain.o src/InformationResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/InformationResource.o ${OBJECTDIR}/src/InformationResource_nomain.o;\
	fi

${OBJECTDIR}/src/JsonSerializer_nomain.o: ${OBJECTDIR}/src/JsonSerializer.o src/JsonSerializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/JsonSerializer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/JsonSerializer_nomain.o src/JsonSerializer.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/JsonSerializer.o ${OBJECTDIR}/src/JsonSerializer_nomain.o;\
	fi

${OBJECTDIR}/src/MediaResource_nomain.o: ${OBJECTDIR}/src/MediaResource.o src/MediaResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/MediaResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MediaResource_nomain.o src/MediaResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/MediaResource.o ${OBJECTDIR}/src/MediaResource_nomain.o;\
	fi

${OBJECTDIR}/src/MessageResource_nomain.o: ${OBJECTDIR}/src/MessageResource.o src/MessageResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/MessageResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MessageResource_nomain.o src/MessageResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/MessageResource.o ${OBJECTDIR}/src/MessageResource_nomain.o;\
	fi

${OBJECTDIR}/src/OptionResource_nomain.o: ${OBJECTDIR}/src/OptionResource.o src/OptionResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/OptionResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/OptionResource_nomain.o src/OptionResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/OptionResource.o ${OBJECTDIR}/src/OptionResource_nomain.o;\
	fi

${OBJECTDIR}/src/PluginResource_nomain.o: ${OBJECTDIR}/src/PluginResource.o src/PluginResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/PluginResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PluginResource_nomain.o src/PluginResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/PluginResource.o ${OBJECTDIR}/src/PluginResource_nomain.o;\
	fi

${OBJECTDIR}/src/ProbeResource_nomain.o: ${OBJECTDIR}/src/ProbeResource.o src/ProbeResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/ProbeResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProbeResource_nomain.o src/ProbeResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/ProbeResource.o ${OBJECTDIR}/src/ProbeResource_nomain.o;\
	fi

${OBJECTDIR}/src/PublicApiResource_nomain.o: ${OBJECTDIR}/src/PublicApiResource.o src/PublicApiResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/PublicApiResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PublicApiResource_nomain.o src/PublicApiResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/PublicApiResource.o ${OBJECTDIR}/src/PublicApiResource_nomain.o;\
	fi

${OBJECTDIR}/src/RoleResource_nomain.o: ${OBJECTDIR}/src/RoleResource.o src/RoleResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/RoleResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/RoleResource_nomain.o src/RoleResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/RoleResource.o ${OBJECTDIR}/src/RoleResource_nomain.o;\
	fi

${OBJECTDIR}/src/SearchResource_nomain.o: ${OBJECTDIR}/src/SearchResource.o src/SearchResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/SearchResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SearchResource_nomain.o src/SearchResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/SearchResource.o ${OBJECTDIR}/src/SearchResource_nomain.o;\
	fi

${OBJECTDIR}/src/SourceResource_nomain.o: ${OBJECTDIR}/src/SourceResource.o src/SourceResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/SourceResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SourceResource_nomain.o src/SourceResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/SourceResource.o ${OBJECTDIR}/src/SourceResource_nomain.o;\
	fi

${OBJECTDIR}/src/UnitResource_nomain.o: ${OBJECTDIR}/src/UnitResource.o src/UnitResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/UnitResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UnitResource_nomain.o src/UnitResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/UnitResource.o ${OBJECTDIR}/src/UnitResource_nomain.o;\
	fi

${OBJECTDIR}/src/UserResource_nomain.o: ${OBJECTDIR}/src/UserResource.o src/UserResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/UserResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UserResource_nomain.o src/UserResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/UserResource.o ${OBJECTDIR}/src/UserResource_nomain.o;\
	fi

${OBJECTDIR}/src/itooki/ItookiAckReceiver_nomain.o: ${OBJECTDIR}/src/itooki/ItookiAckReceiver.o src/itooki/ItookiAckReceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/itooki/ItookiAckReceiver.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiAckReceiver_nomain.o src/itooki/ItookiAckReceiver.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/itooki/ItookiAckReceiver.o ${OBJECTDIR}/src/itooki/ItookiAckReceiver_nomain.o;\
	fi

${OBJECTDIR}/src/itooki/ItookiAswReceiver_nomain.o: ${OBJECTDIR}/src/itooki/ItookiAswReceiver.o src/itooki/ItookiAswReceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/itooki/ItookiAswReceiver.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiAswReceiver_nomain.o src/itooki/ItookiAswReceiver.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/itooki/ItookiAswReceiver.o ${OBJECTDIR}/src/itooki/ItookiAswReceiver_nomain.o;\
	fi

${OBJECTDIR}/src/itooki/ItookiSMSSender_nomain.o: ${OBJECTDIR}/src/itooki/ItookiSMSSender.o src/itooki/ItookiSMSSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/itooki/ItookiSMSSender.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiSMSSender_nomain.o src/itooki/ItookiSMSSender.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/itooki/ItookiSMSSender.o ${OBJECTDIR}/src/itooki/ItookiSMSSender_nomain.o;\
	fi

${OBJECTDIR}/src/itooki/ItookiSendedReiceiver_nomain.o: ${OBJECTDIR}/src/itooki/ItookiSendedReiceiver.o src/itooki/ItookiSendedReiceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/itooki/ItookiSendedReiceiver.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/ItookiSendedReiceiver_nomain.o src/itooki/ItookiSendedReiceiver.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/itooki/ItookiSendedReiceiver.o ${OBJECTDIR}/src/itooki/ItookiSendedReiceiver_nomain.o;\
	fi

${OBJECTDIR}/src/itooki/PublicItookiResource_nomain.o: ${OBJECTDIR}/src/itooki/PublicItookiResource.o src/itooki/PublicItookiResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/itooki/PublicItookiResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/itooki/PublicItookiResource_nomain.o src/itooki/PublicItookiResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/itooki/PublicItookiResource.o ${OBJECTDIR}/src/itooki/PublicItookiResource_nomain.o;\
	fi

${OBJECTDIR}/src/mail/PublicMailResource_nomain.o: ${OBJECTDIR}/src/mail/PublicMailResource.o src/mail/PublicMailResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/mail/PublicMailResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/PublicMailResource_nomain.o src/mail/PublicMailResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/mail/PublicMailResource.o ${OBJECTDIR}/src/mail/PublicMailResource_nomain.o;\
	fi

${OBJECTDIR}/src/mail/mailAssign_nomain.o: ${OBJECTDIR}/src/mail/mailAssign.o src/mail/mailAssign.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/mail/mailAssign.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/mailAssign_nomain.o src/mail/mailAssign.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/mail/mailAssign.o ${OBJECTDIR}/src/mail/mailAssign_nomain.o;\
	fi

${OBJECTDIR}/src/mail/mailForward_nomain.o: ${OBJECTDIR}/src/mail/mailForward.o src/mail/mailForward.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/mail/mailForward.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/mailForward_nomain.o src/mail/mailForward.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/mail/mailForward.o ${OBJECTDIR}/src/mail/mailForward_nomain.o;\
	fi

${OBJECTDIR}/src/mail/mailResolve_nomain.o: ${OBJECTDIR}/src/mail/mailResolve.o src/mail/mailResolve.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/mail
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/mail/mailResolve.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-${target}/workspace/include -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mail/mailResolve_nomain.o src/mail/mailResolve.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/mail/mailResolve.o ${OBJECTDIR}/src/mail/mailResolve_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f2 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
