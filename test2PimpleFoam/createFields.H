#include "createRDeltaT.H"
Info<< "Reading transportProperties\n" << endl;

IOdictionary transportProperties
(
    IOobject
    (
        "transportProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ_IF_MODIFIED,
        IOobject::NO_WRITE
    )
);

dimensionedScalar nu
(
    "nu",
    dimViscosity,
    transportProperties.lookup("nu")
);
dimensionedScalar gammaPsi
(
    "gammaPsi",
    dimViscosity,
    transportProperties.lookup("gammaPsi")
);
dimensionedScalar time_scalar
(
    "time_scalar",
    dimTime,
    transportProperties.lookup("time_scalar")
);
Info<< "Reading field C\n" << endl;
volScalarField C
 (
     IOobject
     (
         "C",
          runTime.timeName(),
          mesh,
          IOobject::MUST_READ,
          IOobject::AUTO_WRITE
     ),
     mesh
 );     
Info<< "Reading field p\n" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading/calculating face flux field phi\n" << endl;

surfaceScalarField phi
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    fvc::flux(U)
);
//create my variable velocity......................
Info<< "up calculating\n" << endl;
volVectorField up
(
    IOobject
    (
        "up",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh //up=(U-time_scalar*(U & fvc::grad(U)));
);

//up=(U-time_scalar*(U & fvc::grad(U)));

//create my variable myphi......................
Info<< "Reading/calculating face flux field myphi\n" << endl;

surfaceScalarField phiup
(
    IOobject
    (
        "phiup",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
   fvc::flux(up)
);
//fraction calculation
surfaceScalarField Cdot
(
    IOobject
    (
        "Cdot",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
   fvc::flux(C*up)
);

pressureReference pressureReference(p, pimple.dict());

mesh.setFluxRequired(p.name());


singlePhaseTransportModel laminarTransport(U, phi);

autoPtr<incompressible::momentumTransportModel> turbulence
(
    incompressible::momentumTransportModel::New(U, phi, laminarTransport)
);

#include "createMRF.H"
#include "createFvModels.H"
#include "createFvConstraints.H"
