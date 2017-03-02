==========================
Developement and internals
==========================

.. toctree::
   :maxdepth: 3

   RULES.rst
   rfcs.rst

Internal API
============

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/buffer.h.rst
   src/chirp.h.rst
   src/chirp.c.rst
   inc/libchirp/common.h.rst
   src/connection.h.rst
   src/connection.c.rst
   src/encryption.h.rst
   src/encryption.c.rst
   src/message.h.rst
   src/message.c.rst
   src/protocol.h.rst
   src/protocol.c.rst
   src/quickcheck.h.rst
   src/quickcheck.c.rst
   src/reader.h.rst
   src/reader.c.rst
   src/util.h.rst
   src/util.c.rst
   src/writer.h.rst
   src/writer.c.rst

Test binaries
=============

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/chirp_etest.c.rst
   src/quickcheck_etest.c.rst

External Libs
=============

* `SGLIB - Simple Generic Library`_

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/sglib.h.rst

.. _`SGLIB - Simple Generic Library`: sglib.html
