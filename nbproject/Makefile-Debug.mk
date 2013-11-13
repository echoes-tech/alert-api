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
CCC=DISTCC_FALLBACK=0 distcc g++-4.7
CXX=DISTCC_FALLBACK=0 distcc g++-4.7
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
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
	${OBJECTDIR}/src/InformationResource.o \
	${OBJECTDIR}/src/MediaResource.o \
	${OBJECTDIR}/src/OrganizationResource.o \
	${OBJECTDIR}/src/PluginResource.o \
	${OBJECTDIR}/src/ProbeResource.o \
	${OBJECTDIR}/src/PublicApiResource.o \
	${OBJECTDIR}/src/RoleResource.o \
	${OBJECTDIR}/src/SearchTypeResource.o \
	${OBJECTDIR}/src/UnitResource.o \
	${OBJECTDIR}/src/UserResource.o \
	${OBJECTDIR}/src/itooki/ItookiSMSSender.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f2

# C Compiler Flags
CFLAGS=`cppunit-config --cflags` 

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../dbo/dist/Debug_SharedObject/GNU-Linux-x86 -ldbo -lwtdbo -lwtdbopostgres -lboost_system -lboost_signals -lwt -lstdc++

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${TESTDIR}/TestFiles/f1

${TESTDIR}/TestFiles/f1: ${OBJECTFILES}
	${MKDIR} -p ${TESTDIR}/TestFiles
	distcc -o ${TESTDIR}/TestFiles/f1 ${OBJECTFILES} ${LDLIBSOPTIONS} -lwthttp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/AddonResource.o: src/AddonResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AddonResource.o src/AddonResource.cpp

${OBJECTDIR}/src/AlertResource.o: src/AlertResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AlertResource.o src/AlertResource.cpp

${OBJECTDIR}/src/AssetResource.o: src/AssetResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AssetResource.o src/AssetResource.cpp

${OBJECTDIR}/src/Conf.o: src/Conf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Conf.o src/Conf.cpp

${OBJECTDIR}/src/CriterionResource.o: src/CriterionResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CriterionResource.o src/CriterionResource.cpp

${OBJECTDIR}/src/InformationResource.o: src/InformationResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/InformationResource.o src/InformationResource.cpp

${OBJECTDIR}/src/MediaResource.o: src/MediaResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MediaResource.o src/MediaResource.cpp

${OBJECTDIR}/src/OrganizationResource.o: src/OrganizationResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/OrganizationResource.o src/OrganizationResource.cpp

${OBJECTDIR}/src/PluginResource.o: src/PluginResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PluginResource.o src/PluginResource.cpp

${OBJECTDIR}/src/ProbeResource.o: src/ProbeResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ProbeResource.o src/ProbeResource.cpp

${OBJECTDIR}/src/PublicApiResource.o: src/PublicApiResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PublicApiResource.o src/PublicApiResource.cpp

${OBJECTDIR}/src/RoleResource.o: src/RoleResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/RoleResource.o src/RoleResource.cpp

${OBJECTDIR}/src/SearchTypeResource.o: src/SearchTypeResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/SearchTypeResource.o src/SearchTypeResource.cpp

${OBJECTDIR}/src/UnitResource.o: src/UnitResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/UnitResource.o src/UnitResource.cpp

${OBJECTDIR}/src/UserResource.o: src/UserResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/UserResource.o src/UserResource.cpp

${OBJECTDIR}/src/itooki/ItookiSMSSender.o: src/itooki/ItookiSMSSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/itooki/ItookiSMSSender.o src/itooki/ItookiSMSSender.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f2: ${TESTDIR}/src/tests/AddonResourceTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS} -lwttest -lboost_unit_test_framework -lboost_thread 


${TESTDIR}/src/tests/AddonResourceTest.o: src/tests/AddonResourceTest.cpp 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -I. -std=c++11 -DBOOST_TEST_MAIN -MMD -MP -MF $@.d -o ${TESTDIR}/src/tests/AddonResourceTest.o src/tests/AddonResourceTest.cpp


${OBJECTDIR}/main_nomain.o: ${OBJECTDIR}/main.o main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/main_nomain.o main.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AddonResource_nomain.o src/AddonResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AlertResource_nomain.o src/AlertResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AssetResource_nomain.o src/AssetResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Conf_nomain.o src/Conf.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CriterionResource_nomain.o src/CriterionResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/CriterionResource.o ${OBJECTDIR}/src/CriterionResource_nomain.o;\
	fi

${OBJECTDIR}/src/InformationResource_nomain.o: ${OBJECTDIR}/src/InformationResource.o src/InformationResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/InformationResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/InformationResource_nomain.o src/InformationResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/InformationResource.o ${OBJECTDIR}/src/InformationResource_nomain.o;\
	fi

${OBJECTDIR}/src/MediaResource_nomain.o: ${OBJECTDIR}/src/MediaResource.o src/MediaResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/MediaResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MediaResource_nomain.o src/MediaResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/MediaResource.o ${OBJECTDIR}/src/MediaResource_nomain.o;\
	fi

${OBJECTDIR}/src/OrganizationResource_nomain.o: ${OBJECTDIR}/src/OrganizationResource.o src/OrganizationResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/OrganizationResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/OrganizationResource_nomain.o src/OrganizationResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/OrganizationResource.o ${OBJECTDIR}/src/OrganizationResource_nomain.o;\
	fi

${OBJECTDIR}/src/PluginResource_nomain.o: ${OBJECTDIR}/src/PluginResource.o src/PluginResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/PluginResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PluginResource_nomain.o src/PluginResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ProbeResource_nomain.o src/ProbeResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PublicApiResource_nomain.o src/PublicApiResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/RoleResource_nomain.o src/RoleResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/RoleResource.o ${OBJECTDIR}/src/RoleResource_nomain.o;\
	fi

${OBJECTDIR}/src/SearchTypeResource_nomain.o: ${OBJECTDIR}/src/SearchTypeResource.o src/SearchTypeResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/SearchTypeResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/SearchTypeResource_nomain.o src/SearchTypeResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/SearchTypeResource.o ${OBJECTDIR}/src/SearchTypeResource_nomain.o;\
	fi

${OBJECTDIR}/src/UnitResource_nomain.o: ${OBJECTDIR}/src/UnitResource.o src/UnitResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/UnitResource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/UnitResource_nomain.o src/UnitResource.cpp;\
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
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/UserResource_nomain.o src/UserResource.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/UserResource.o ${OBJECTDIR}/src/UserResource_nomain.o;\
	fi

${OBJECTDIR}/src/itooki/ItookiSMSSender_nomain.o: ${OBJECTDIR}/src/itooki/ItookiSMSSender.o src/itooki/ItookiSMSSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/itooki/ItookiSMSSender.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -g -Wall -Iinclude -I../dbo/include -I. -I../wt-3.3.1-rc2/src/web -I. -std=c++11 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/itooki/ItookiSMSSender_nomain.o src/itooki/ItookiSMSSender.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/src/itooki/ItookiSMSSender.o ${OBJECTDIR}/src/itooki/ItookiSMSSender_nomain.o;\
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
	${RM} ${TESTDIR}/TestFiles/f1

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
