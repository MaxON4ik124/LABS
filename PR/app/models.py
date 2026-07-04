from __future__ import annotations

from datetime import datetime, timezone

from flask_sqlalchemy import SQLAlchemy
from sqlalchemy import DateTime, ForeignKey, String, Text
from sqlalchemy.orm import Mapped, mapped_column, relationship


db = SQLAlchemy()


class User(db.Model):
    __tablename__ = "users"

    id: Mapped[int] = mapped_column(
        primary_key=True,
        autoincrement=True,
    )

    username: Mapped[str] = mapped_column(
        String(50),
        unique=True,
        nullable=False,
        index=True,
    )

    password_hash: Mapped[str] = mapped_column(
        String(255),
        nullable=False,
    )

    balance: Mapped[int] = mapped_column(
        default=1000,
        nullable=False,
    )

    orders: Mapped[list["Order"]] = relationship(
        back_populates="user",
        cascade="all, delete-orphan",
    )


class Product(db.Model):
    __tablename__ = "products"

    id: Mapped[str] = mapped_column(
        String(50),
        primary_key=True,
    )

    name: Mapped[str] = mapped_column(
        String(120),
        nullable=False,
    )

    description: Mapped[str] = mapped_column(
        Text,
        default="",
        nullable=False,
    )

    price: Mapped[int] = mapped_column(
        nullable=False,
    )

    image_path: Mapped[str] = mapped_column(
        String(255),
        nullable=False,
    )

    orders: Mapped[list["Order"]] = relationship(
        back_populates="product",
    )


class Order(db.Model):
    __tablename__ = "orders"

    id: Mapped[int] = mapped_column(
        primary_key=True,
        autoincrement=True,
    )

    user_id: Mapped[int] = mapped_column(
        ForeignKey(
            "users.id",
            ondelete="CASCADE",
        ),
        nullable=False,
        index=True,
    )

    product_id: Mapped[str] = mapped_column(
        ForeignKey("products.id"),
        nullable=False,
        index=True,
    )

    product_name: Mapped[str] = mapped_column(
        String(120),
        nullable=False,
    )

    unit_price: Mapped[int] = mapped_column(
        nullable=False,
    )

    quantity: Mapped[int] = mapped_column(
        nullable=False,
    )

    total: Mapped[int] = mapped_column(
        nullable=False,
    )

    comment: Mapped[str] = mapped_column(
        Text,
        default="",
        nullable=False,
    )

    created_at: Mapped[datetime] = mapped_column(
        DateTime(timezone=True),
        default=lambda: datetime.now(timezone.utc),
        nullable=False,
    )

    user: Mapped["User"] = relationship(
        back_populates="orders",
    )

    product: Mapped["Product"] = relationship(
        back_populates="orders",
    )