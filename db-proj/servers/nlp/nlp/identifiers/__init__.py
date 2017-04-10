from relationship import is_single_relationship
import statistics
import membership
import relationship

def identify(tokens):
    is_error = False

    api_call, params, found = membership.identify(tokens)
    if found:
        return api_call, params, is_error

    api_call, params, found = relationship.identify(tokens)
    if found:
        return api_call, params, is_error

    api_call, params, found = statistics.identify(tokens)
    if found:
        return api_call, params, is_error

    is_error = True
    return '', {}, is_error

