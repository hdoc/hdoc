// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

// HDOC_GIT_REV takes the value of the git revision of the repo
// or the value "dirty" when built with nix flakes
// When built on a dev machine with meson+ninja, HDOC_GIT_REV is
// undefined and consequently gets the default value of hdocInternal
#ifndef HDOC_GIT_REV
#define HDOC_GIT_REV "hdocInternal"
#endif

#define HDOC_NUMERIC_VERSION "1.2.2"
#define HDOC_VERSION HDOC_NUMERIC_VERSION "-" HDOC_GIT_REV
