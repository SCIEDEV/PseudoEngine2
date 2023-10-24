#include "pch.h"

#include "nodes/io/file.h"
#include "interpreter/error.h"
#include "interpreter/file.h"

OpenFileNode::OpenFileNode(const Token &token, Node &filename, Interpreter::FileMode mode)
    : Node(token), mode(mode), filename(filename) {}

std::unique_ptr<NodeResult> OpenFileNode::evaluate(Interpreter::Context &ctx) {
    auto filenameRes = filename.evaluate(ctx);
    if (filenameRes->type != Interpreter::DataType::STRING)
        throw Interpreter::RuntimeError(token, ctx, "Expected string for file name");

    const Interpreter::String &filename = filenameRes->get<Interpreter::String>();

    Interpreter::File *file = ctx.getFileManager().getFile(filename);
    if (file != nullptr)
        throw Interpreter::RuntimeError(token, ctx, "File '" + filename.value + "' is already open");

    bool success = ctx.getFileManager().createFile(filename, mode);
    if (!success)
        throw Interpreter::RuntimeError(token, ctx, "Failed to open file '" + filename.value + "'");

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


ReadFileNode::ReadFileNode(const Token &token, Node &filename, const Token &identifier)
    : Node(token), filename(filename), identifier(identifier) {}

std::unique_ptr<NodeResult> ReadFileNode::evaluate(Interpreter::Context &ctx) {
    auto filenameRes = filename.evaluate(ctx);
    if (filenameRes->type != Interpreter::DataType::STRING)
        throw Interpreter::RuntimeError(token, ctx, "Expected string for file name");
    
    Interpreter::Variable *var = ctx.getVariable(identifier.value);
    if (var == nullptr) {
        var = new Interpreter::Variable(identifier.value, Interpreter::DataType::STRING, false, &ctx);
        ctx.addVariable(var);
    }
    if (var->type != Interpreter::DataType::STRING)
        throw Interpreter::RuntimeError(token, ctx, "Variable of type STRING expected");

    auto &filename = filenameRes->get<Interpreter::String>();
    Interpreter::File *file = ctx.getFileManager().getFile(filename);
    if (file == nullptr)
        throw Interpreter::FileNotOpenError(token, ctx, filename.value);
    
    Interpreter::String *data = new Interpreter::String;
    *data = file->read();
    var->set(data);

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


WriteFileNode::WriteFileNode(const Token &token, Node &filename, Node &node)
    : UnaryNode(token, node), filename(filename) {}

std::unique_ptr<NodeResult> WriteFileNode::evaluate(Interpreter::Context &ctx) {
    auto filenameRes = filename.evaluate(ctx);
    if (filenameRes->type != Interpreter::DataType::STRING)
        throw Interpreter::RuntimeError(token, ctx, "Expected string for file name");
    
    auto &filename = filenameRes->get<Interpreter::String>();
    Interpreter::File *file = ctx.getFileManager().getFile(filename);
    if (file == nullptr)
        throw Interpreter::FileNotOpenError(token, ctx, filename.value);
    if (file->getMode() == Interpreter::FileMode::READ)
        throw Interpreter::RuntimeError(token, ctx, "File '" + filename.value + "' is opened as read-only");

    auto nodeRes = node.evaluate(ctx);
    std::unique_ptr<Interpreter::String> data;
    switch (nodeRes->type.type) {
        case Interpreter::DataType::INTEGER:
            data = nodeRes->get<Interpreter::Integer>().toString();
            break;
        case Interpreter::DataType::REAL:
            data = nodeRes->get<Interpreter::Real>().toString();
            break;
        case Interpreter::DataType::BOOLEAN:
            data = nodeRes->get<Interpreter::Boolean>().toString();
            break;
        case Interpreter::DataType::CHAR:
            data = nodeRes->get<Interpreter::Char>().toString();
            break;
        case Interpreter::DataType::STRING: {
            auto str = nodeRes->get<Interpreter::String>();
            data = std::make_unique<Interpreter::String>(str);
            break;
        } case Interpreter::DataType::DATE:
            data = nodeRes->get<Interpreter::Date>().toString();
            break;
        case Interpreter::DataType::NONE:
            throw Interpreter::RuntimeError(token, ctx, "Expected value for writing");
        case Interpreter::DataType::ENUM:
        case Interpreter::DataType::POINTER:
        case Interpreter::DataType::COMPOSITE:
            throw Interpreter::TypeOperationError(token, ctx, "Write");
    }

    file->write(*data);

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


CloseFileNode::CloseFileNode(const Token &token, Node &filename)
    : Node(token), filename(filename) {}

std::unique_ptr<NodeResult> CloseFileNode::evaluate(Interpreter::Context &ctx) {
    auto filenameRes = filename.evaluate(ctx);
    if (filenameRes->type != Interpreter::DataType::STRING)
        throw Interpreter::RuntimeError(token, ctx, "Expected string for file name");
    
    auto &filename = filenameRes->get<Interpreter::String>();
    Interpreter::File *file = ctx.getFileManager().getFile(filename);
    if (file == nullptr)
        throw Interpreter::FileNotOpenError(token, ctx, filename.value);

    ctx.getFileManager().closeFile(filename);

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
