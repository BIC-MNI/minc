C----------------------------- MNI Header ------------------------------------
C@NAME       : minc.inc
C@DESCRIPTION: FORTRAN include file for minc (Medical Image NetCDF) 
C              file format standard.
C@METHOD     : Includes name definitions for NetCDF dimensions, attributes
C              and variables for the following :
C                 NetCDF standard attributes
C                 MI general variable attributes
C                 MI dimensions and associated variables
C                 MI dimension variable attributes
C                 MI root variable
C                 MI image variable
C                 MI patient variable
C                 MI study variable
C                 MI acquisition variable
C@CREATED    : March 15, 1993. (Peter Neelin, Montreal Neurological Institute)
C@MODIFIED   : 
C@COPYRIGHT  :
C              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
C              Montreal Neurological Institute, McGill University.
C              Permission to use, copy, modify, and distribute this
C              software and its documentation for any purpose and without
C              fee is hereby granted, provided that the above copyright
C              notice appear in all copies.  The author and McGill University
C              make no representations about the suitability of this
C              software for any purpose.  It is provided "as is" without
C              express or implied warranty.
C@RCSID      : $Header: /private-cvsroot/minc/fortran/Attic/minc.inc,v 4.0 1997-05-07 20:06:39 neelin Rel $ MINC (MNI)
C-----------------------------------------------------------------------------

C     Some useful constants
      PARAMETER MI_EMPTY_STRING=' '
C     Error flags
      INTEGER MI_ERROR, MI_NOERROR
      PARAMETER (MI_ERROR=-1, MI_NOERROR=0)
C     Maximum length of standard attributes
      INTEGER MI_MAX_ATTSTR_LEN
      PARAMETER (MI_MAX_ATTSTR_LEN=64)
C     Number of spatial dimensions
      INTEGER MI_NUM_SPACE_DIMS
      PARAMETER (MI_NUM_SPACE_DIMS=3)
C     Maximum number of image dimensions for image conversion
      INTEGER MI_MAX_IMGDIMS
      PARAMETER (MI_MAX_IMGDIMS=MAXNCDIM)

C     NetCDF standard attributes
      PARAMETER MIUNITS      ='units'
      PARAMETER MILONG_NAME  ='long_name'
      PARAMETER MIVALID_RANGE='valid_range'
      PARAMETER MIVALID_MAX  ='valid_max'
      PARAMETER MIVALID_MIN  ='valid_min'
      PARAMETER MI_FILLVALUE ='_Fillvalue'
      PARAMETER MITITLE      ='title'
      PARAMETER MIHISTORY    ='history'

C     General variable attributes
      PARAMETER MIVARTYPE ='vartype'
      PARAMETER MIVARID   ='varid'
      PARAMETER MISIGNTYPE='signtype'
      PARAMETER MIPARENT  ='parent'
      PARAMETER MICHILDREN='children'
      PARAMETER MICOMMENTS='comments'
      PARAMETER MIVERSION ='version'

C     General attribute constants
C     Prefix for identifying a variable attribute pointer
      PARAMETER MI_VARATT_POINTER_PREFIX ='--->'
C     Separator for elements of MIchildren
      PARAMETER MI_CHILD_SEPARATOR='\n'
C     MIvartype values
      PARAMETER MI_GROUP    ='group________'
      PARAMETER MI_DIMENSION='dimension____'
      PARAMETER MI_DIM_WIDTH='dim-width____'
      PARAMETER MI_VARATT   ='var_attribute'
C     MIvarid value
      PARAMETER MI_STDVAR='MINC standard variable'
C     MIsigntype values
      PARAMETER MI_SIGNED  ='signed__'
      PARAMETER MI_UNSIGNED='unsigned'
C     MIversion value
      PARAMETER MI_VERSION_1_0='MINC Version    1.0'
      PARAMETER MI_CURRENT_VERSION=MI_VERSION_1_0
C     Generally useful values for boolean attributes
      PARAMETER MI_TRUE ='true_'
      PARAMETER MI_FALSE='false'

C     Dimension names and names of associated variables
      PARAMETER MIXSPACE          ='xspace'
      PARAMETER MIYSPACE          ='yspace'
      PARAMETER MIZSPACE          ='zspace'
      PARAMETER MITIME            ='time'
      PARAMETER MITFREQUENCY      ='tfrequency'
      PARAMETER MIXFREQUENCY      ='xfrequency'
      PARAMETER MIYFREQUENCY      ='yfrequency'
      PARAMETER MIZFREQUENCY      ='zfrequency'
      PARAMETER MIVECTOR_DIMENSION='vector_dimension'
      PARAMETER MIXSPACE_WIDTH    ='xspace-width'
      PARAMETER MIYSPACE_WIDTH    ='yspace-width'
      PARAMETER MIZSPACE_WIDTH    ='zspace-width'
      PARAMETER MITIME_WIDTH      ='time-width'
      PARAMETER MITFREQUENCY_WIDTH='tfrequency-width'
      PARAMETER MIXFREQUENCY_WIDTH='xfrequency-width'
      PARAMETER MIYFREQUENCY_WIDTH='yfrequency-width'
      PARAMETER MIZFREQUENCY_WIDTH='zfrequency-width'

C     Dimension variable attribute names
C     For dimension variables (MIspacing is also for dimension width vars)
      PARAMETER MISPACING          ='spacing'
      PARAMETER MISTEP             ='step'
      PARAMETER MISTART            ='start'
      PARAMETER MISPACETYPE        ='spacetype'
      PARAMETER MIALIGNMENT        ='alignment'
      PARAMETER MIDIRECTION_COSINES='direction_cosines'
C     For dimension width variables
      PARAMETER MIWIDTH            ='width'
      PARAMETER MIFILTERTYPE       ='filtertype'

C     Dimension attribute constants
C     MIgridtype values
      PARAMETER MI_REGULAR  ='regular__'
      PARAMETER MI_IRREGULAR='irregular'
C     MIspacetype values
      PARAMETER MI_NATIVE   ='native____'
      PARAMETER MI_TALAIRACH='talairach_'
      PARAMETER MI_CALLOSAL ='callosal__'
C     MIalignment values
      PARAMETER MI_START ='start_'
      PARAMETER MI_CENTRE='centre'
      PARAMETER MI_END   ='end___'
      PARAMETER MI_CENTER=MI_CENTRE
C     MIfiltertype values
      PARAMETER MI_SQUARE    ='square____'
      PARAMETER MI_GAUSSIAN  ='gaussian__'
      PARAMETER MI_TRIANGULAR='triangular'

C     The root variable
      PARAMETER MIROOTVARIABLE='rootvariable'

C     The image variable and its attributes
      PARAMETER MIIMAGE   ='image'
      PARAMETER MIIMAGEMAX='image-max'
      PARAMETER MIIMAGEMIN='image-min'
      PARAMETER MICOMPLETE='complete'

C     The patient variable and its attributes
      PARAMETER MIPATIENT       ='patient'
      PARAMETER MIFULL_NAME     ='full_name'
      PARAMETER MIOTHER_NAMES   ='other_names'
      PARAMETER MIIDENTIFICATION='identification'
      PARAMETER MIOTHER_IDS     ='other_ids'
      PARAMETER MIBIRTHDATE     ='birthdate'
      PARAMETER MISEX           ='sex'
      PARAMETER MIAGE           ='age'
      PARAMETER MIWEIGHT        ='weight'
      PARAMETER MISIZE          ='size'
      PARAMETER MIADDRESS       ='address'
      PARAMETER MIINSURANCE_ID  ='insurance_id'

C     Patient attribute constants
      PARAMETER MI_MALE  ='male__'
      PARAMETER MI_FEMALE='female'
      PARAMETER MI_OTHER ='other_'

C     The study variable and its attributes
      PARAMETER MISTUDY              ='study'
      PARAMETER MISTART_TIME         ='start_time'
      PARAMETER MISTART_YEAR         ='start_year'
      PARAMETER MISTART_MONTH        ='start_month'
      PARAMETER MISTART_DAY          ='start_day'
      PARAMETER MISTART_HOUR         ='start_hour'
      PARAMETER MISTART_MINUTE       ='start_minute'
      PARAMETER MISTART_SECONDS      ='start_seconds'
      PARAMETER MIMODALITY           ='modality'
      PARAMETER MIMANUFACTURER       ='manufacturer'
      PARAMETER MIDEVICE_MODEL       ='device_model'
      PARAMETER MIINSTITUTION        ='institution'
      PARAMETER MIDEPARTMENT         ='department'
      PARAMETER MISTATION_ID         ='station_id'
      PARAMETER MIREFERRING_PHYSICIAN='referring_physician'
      PARAMETER MIATTENDING_PHYSICIAN='attending_physician'
      PARAMETER MIRADIOLOGIST        ='radiologist'
      PARAMETER MIOPERATOR           ='operator'
      PARAMETER MIADMITTING_DIAGNOSIS='admitting_diagnosis'
      PARAMETER MIPROCEDURE          ='procedure'
      PARAMETER MISTUDY_ID           ='study_id'

C     Study attribute constants
      PARAMETER MI_PET  ='PET__'
      PARAMETER MI_SPECT='SPECT'
      PARAMETER MI_GAMMA='GAMMA'
      PARAMETER MI_MRI  ='MRI__'
      PARAMETER MI_MRS  ='MRS__'
      PARAMETER MI_MRA  ='MRA__'
      PARAMETER MI_CT   ='CT___'
      PARAMETER MI_DSA  ='DSA__'
      PARAMETER MI_DR   ='DR___'

C     The acquisition variable and its attributes
      PARAMETER MIACQUISITION          ='acquisition'
      PARAMETER MIPROTOCOL             ='protocol'
      PARAMETER MISCANNING_SEQUENCE    ='scanning_sequence'
      PARAMETER MIREPETITION_TIME      ='repetition_time'
      PARAMETER MIECHO_TIME            ='echo_time'
      PARAMETER MIINVERSION_TIME       ='inversion_time'
      PARAMETER MINUM_AVERAGES         ='num_averages'
      PARAMETER MIIMAGING_FREQUENCY    ='imaging_frequency'
      PARAMETER MIIMAGED_NUCLEUS       ='imaged_nucleus'
      PARAMETER MIRADIONUCLIDE         ='radionuclide'
      PARAMETER MICONTRAST_AGENT       ='contrast_agent'
      PARAMETER MIRADIONUCLIDE_HALFLIFE='radionuclide_halflife'
      PARAMETER MITRACER               ='tracer'
      PARAMETER MIINJECTION_TIME       ='injection_time'
      PARAMETER MIINJECTION_YEAR       ='injection_year'
      PARAMETER MIINJECTION_MONTH      ='injection_month'
      PARAMETER MIINJECTION_DAY        ='injection_day'
      PARAMETER MIINJECTION_HOUR       ='injection_hour'
      PARAMETER MIINJECTION_MINUTE     ='injection_minute'
      PARAMETER MIINJECTION_SECONDS    ='injection_seconds'
      PARAMETER MIINJECTION_LENGTH     ='injection_length'
      PARAMETER MIINJECTION_DOSE       ='injection_dose'
      PARAMETER MIDOSE_UNITS           ='dose_units'
      PARAMETER MIINJECTION_VOLUME     ='injection_volume'
      PARAMETER MIINJECTION_ROUTE      ='injection_route'

C     Constants for image conversion variable (icv) properties
C     Maximum number of icv's allowed
      INTEGER MI_MAX_NUM_ICV
      PARAMETER (MI_MAX_NUM_ICV=MAXNCOP)
C     Default max and min for normalization
      DOUBLE PRECISION MI_DEFAULT_MAX, MI_DEFAULT_MIN
      PARAMETER (MI_DEFAULT_MAX=1.0)
      PARAMETER (MI_DEFAULT_MIN=0.0)

C     Icv properties
      INTEGER MI_ICV_TYPE, MI_ICV_SIGN, MI_ICV_DO_RANGE
      INTEGER MI_ICV_VALID_MAX, MI_ICV_VALID_MIN, MI_ICV_DO_NORM
      INTEGER MI_ICV_USER_NORM, MI_ICV_IMAGE_MAX, MI_ICV_IMAGE_MIN
      INTEGER MI_ICV_NORM_MAX, MI_ICV_NORM_MIN, MI_ICV_DO_DIM_CONV
      INTEGER MI_ICV_DO_SCALAR, MI_ICV_XDIM_DIR, MI_ICV_YDIM_DIR
      INTEGER MI_ICV_ZDIM_DIR, MI_ICV_ADIM_SIZE, MI_ICV_BDIM_SIZE
      INTEGER MI_ICV_KEEP_ASPECT, MI_ICV_ADIM_STEP, MI_ICV_BDIM_STEP
      INTEGER MI_ICV_ADIM_START, MI_ICV_BDIM_START, MI_ICV_NUM_IMGDIMS
      INTEGER MI_ICV_NUM_DIMS, MI_ICV_CDFID, MI_ICV_VARID
      INTEGER MI_ICV_MAXVAR, MI_ICV_MINVAR, MI_ICV_DIM_SIZE
      INTEGER MI_ICV_DIM_STEP, MI_ICV_DIM_START
      INTEGER MI_ICV_DO_FILLVALUE, MI_ICV_FILLVALUE

C     For converting data type
      PARAMETER (MI_ICV_TYPE            =1)
      PARAMETER (MI_ICV_SIGN            =2)
      PARAMETER (MI_ICV_DO_RANGE        =3)
      PARAMETER (MI_ICV_VALID_MAX       =4)
      PARAMETER (MI_ICV_VALID_MIN       =5)
C     For doing normalization
      PARAMETER (MI_ICV_DO_NORM         =6)
      PARAMETER (MI_ICV_USER_NORM       =7)
      PARAMETER (MI_ICV_IMAGE_MAX       =8)
      PARAMETER (MI_ICV_IMAGE_MIN       =9)
C     Values actually used in normalization - read-only
      PARAMETER (MI_ICV_NORM_MAX       =10)
      PARAMETER (MI_ICV_NORM_MIN       =11)
C     For doing dimension conversions
      PARAMETER (MI_ICV_DO_DIM_CONV    =12)
C     For converting vector fields to scalar
      PARAMETER (MI_ICV_DO_SCALAR      =13)
C     For flipping axis direction
      PARAMETER (MI_ICV_XDIM_DIR       =14)
      PARAMETER (MI_ICV_YDIM_DIR       =15)
      PARAMETER (MI_ICV_ZDIM_DIR       =16)
C     For changing size of first two dimensions (excluding mivector_dimension)
      PARAMETER (MI_ICV_ADIM_SIZE      =17)
      PARAMETER (MI_ICV_BDIM_SIZE      =18)
      PARAMETER (MI_ICV_KEEP_ASPECT    =19)
C     The pixel size and location of first two dimensions (these are readonly)
      PARAMETER (MI_ICV_ADIM_STEP      =20)
      PARAMETER (MI_ICV_BDIM_STEP      =21)
      PARAMETER (MI_ICV_ADIM_START     =22)
      PARAMETER (MI_ICV_BDIM_START     =23)
C     Number of image dimensions for dimension conversion
      PARAMETER (MI_ICV_NUM_IMGDIMS    =24)
C     Number of dimensions of image variable taking into account vector/scalar
C     data (read-only property)
      PARAMETER (MI_ICV_NUM_DIMS       =25)
C     Id of file and image variable (read-only properties)
      PARAMETER (MI_ICV_CDFID          =26)
      PARAMETER (MI_ICV_VARID          =27)
C     Names of MIimagemax and MIimagemin variables
      PARAMETER (MI_ICV_MAXVAR         =28)
      PARAMETER (MI_ICV_MINVAR         =29)
C     For setting input values to a specified fillvalue
      PARAMETER (MI_ICV_DO_FILLVALUE   =30)
      PARAMETER (MI_ICV_FILLVALUE      =31)
C     Image dimension properties. for each dimension, add the dimension
C     number (counting from fastest to slowest).
      PARAMETER (MI_ICV_DIM_SIZE       =1000)
      PARAMETER (MI_ICV_DIM_STEP       =1100)
      PARAMETER (MI_ICV_DIM_START      =1200)

C     Constants that can be used as values for the above properties.
C     possible values for mi_icv_?dim_dir
      INTEGER MI_ICV_POSITIVE, MI_ICV_NEGATIVE, MI_ICV_ANYDIR
      PARAMETER (MI_ICV_POSITIVE        =1)
      PARAMETER (MI_ICV_NEGATIVE        =-1)
      PARAMETER (MI_ICV_ANYDIR          =0)
C     Possible value for mi_icv_?dim_size
      INTEGER MI_ICV_ANYSIZE
      PARAMETER (MI_ICV_ANYSIZE         =-1)

C     Error codes
C     Note that they must not conflict with netcdf error codes since
C     they are stored in the same global variable.
      INTEGER MI_ERR_NONNUMERIC, MI_ERR_NONCHAR, MI_ERR_NONSCALAR
      INTEGER MI_ERR_BADOP, MI_ERR_NOTPOINTER, MI_ERR_BAD_STDVAR
      INTEGER MI_ERR_BADSUFFIX, MI_ERR_NOICV, MI_ERR_BADICV
      INTEGER MI_ERR_BADPROP, MI_ERR_ICVATTACHED, MI_ERR_TOOFEWDIMS
      INTEGER MI_ERR_ICVNOTATTACHED, MI_ERR_DIMSIZE
      INTEGER MI_ERR_ICV_INVCOORDS, MI_ERR_WRONGNDIMS, MI_ERR_BADMATCH
      INTEGER MI_ERR_MAXMIN_DIMS

      PARAMETER (MI_ERR_NONNUMERIC      =1331)
      PARAMETER (MI_ERR_NONCHAR         =1332)
      PARAMETER (MI_ERR_NONSCALAR       =1333)
      PARAMETER (MI_ERR_BADOP           =1334)
      PARAMETER (MI_ERR_NOTPOINTER      =1335)
      PARAMETER (MI_ERR_BAD_STDVAR      =1336)
      PARAMETER (MI_ERR_BADSUFFIX       =1337)
      PARAMETER (MI_ERR_NOICV           =1338)
      PARAMETER (MI_ERR_BADICV          =1339)
      PARAMETER (MI_ERR_BADPROP         =1340)
      PARAMETER (MI_ERR_ICVATTACHED     =1341)
      PARAMETER (MI_ERR_TOOFEWDIMS      =1342)
      PARAMETER (MI_ERR_ICVNOTATTACHED  =1343)
      PARAMETER (MI_ERR_DIMSIZE         =1344)
      PARAMETER (MI_ERR_ICV_INVCOORDS   =1345)
      PARAMETER (MI_ERR_WRONGNDIMS      =1346)
      PARAMETER (MI_ERR_BADMATCH        =1347)
      PARAMETER (MI_ERR_MAXMIN_DIMS     =1348)

C     Function and subroutine declarations
      EXTERNAL MIOPN, MICRE, MICLOSE
      EXTERNAL MIAGT, MIAGT1, MIVGT, MIVGT1, MIVPT, MIVPT1
      EXTERNAL MISTCD, MITRCD, MICAAT, MICVD, MICVV, MICAVD
      EXTERNAL MICAVV, MIAPTP, MIAGTP, MICHLD, MISTDV, MIGRPV
      EXTERNAL ICVCRT, ICVFRE, ICVSTD, ICVSTI, ICVSTS, ICVNQD
      EXTERNAL ICVNQI, ICVNQS, ICVNDA, ICVDET, ICVGET, ICVPUT
      EXTERNAL ICVATT

      INTEGER MIOPN, MICRE
      INTEGER MIAGTP, MICVD, MISTDV, MIGRPV
      INTEGER ICVCRT

