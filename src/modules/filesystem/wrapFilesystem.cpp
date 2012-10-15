namespace love
{
    namespace filesystem
    {
        namespace physfs
        {
            // XXX: Hook required by File class.
            bool hack_setupWriteDirectory() {
                return false;
            }
        }
    }
}
