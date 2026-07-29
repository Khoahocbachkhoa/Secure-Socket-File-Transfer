-- Users
CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password TEXT NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

-- Thư mục
CREATE TABLE folders (
    id BIGSERIAL PRIMARY KEY,
    owner_id BIGINT NOT NULL -- người tạo thư mục
        REFERENCES users(id)
        ON DELETE CASCADE,
    parent_id BIGINT         -- thư mục cha
        REFERENCES folders(id)
        ON DELETE CASCADE,

    name VARCHAR(255) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMP NOT NULL DEFAULT NOW(),
    deleted_at TIMESTAMP
);

CREATE UNIQUE INDEX uq_folder_name -- các thư mục trong cùng 1 dir phải khác tên
ON folders(owner_id, parent_id, name) 
WHERE deleted_at IS NULL;

CREATE UNIQUE INDEX uq_root_folder -- một user chỉ được tạo 1 thư mục gốc
ON folders(owner_id)
WHERE parent_id IS NULL
AND deleted_at IS NULL;

-- Tệp tin
CREATE TABLE files (
    id BIGSERIAL PRIMARY KEY,

    owner_id BIGINT NOT NULL
        REFERENCES users(id)
        ON DELETE CASCADE,

    folder_id BIGINT
        REFERENCES folders(id)
        ON DELETE CASCADE,

    filename VARCHAR(255) NOT NULL,

    storage_key TEXT NOT NULL UNIQUE, -- storage key đơn giản gồm dấu thời gian + ngày

    size BIGINT NOT NULL DEFAULT 0,

    mime_type VARCHAR(255),

    created_at TIMESTAMP NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMP NOT NULL DEFAULT NOW(),
    deleted_at TIMESTAMP
);

CREATE UNIQUE INDEX uq_file_name
ON files(owner_id, folder_id, filename) -- Một thư mục chứa các file có tên riêng biệt
WHERE deleted_at IS NULL;

-- Phân quyền

CREATE TABLE folder_permissions (
    folder_id BIGINT NOT NULL
        REFERENCES folders(id)
        ON DELETE CASCADE,

    user_id BIGINT NOT NULL
        REFERENCES users(id)
        ON DELETE CASCADE,

    role VARCHAR(20) NOT NULL,

    created_at TIMESTAMP DEFAULT NOW(),

    PRIMARY KEY(folder_id, user_id),

    CONSTRAINT check_folder_role
    CHECK (role IN ('viewer', 'editor'))
);

CREATE TABLE file_permissions (
    file_id BIGINT NOT NULL
        REFERENCES files(id)
        ON DELETE CASCADE,

    user_id BIGINT NOT NULL
        REFERENCES users(id)
        ON DELETE CASCADE,

    role VARCHAR(20) NOT NULL,

    created_at TIMESTAMP DEFAULT NOW(),

    PRIMARY KEY(file_id, user_id),

    CONSTRAINT check_file_role
    CHECK (role IN ('viewer', 'editor'))
);

-- audit
CREATE TABLE audit_logs (
    id BIGSERIAL PRIMARY KEY,

    actor_user_id BIGINT
        REFERENCES users(id),

    action VARCHAR(50) NOT NULL,

    resource_type VARCHAR(10) NOT NULL,
    resource_id BIGINT NOT NULL,

    metadata JSONB,

    created_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT check_resource_type
    CHECK (resource_type IN ('file', 'folder'))
);

-- Chỉ mục

CREATE INDEX idx_folder_parent
ON folders(parent_id);

CREATE INDEX idx_file_folder
ON files(folder_id);

CREATE INDEX idx_folder_permission_user
ON folder_permissions(user_id);

CREATE INDEX idx_file_permission_user
ON file_permissions(user_id);

CREATE INDEX idx_audit_resource
ON audit_logs(resource_type, resource_id);

CREATE INDEX idx_folder_owner_parent
ON folders(owner_id, parent_id);

CREATE INDEX idx_file_owner_folder
ON files(owner_id, folder_id);